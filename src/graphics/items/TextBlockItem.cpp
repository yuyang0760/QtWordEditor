#include "graphics/items/TextBlockItem.h"
#include "core/document/ParagraphBlock.h"
#include "core/document/TextSpan.h"
#include "graphics/items/TextBlockLayoutEngine.h"
#include "core/utils/Constants.h"
#include <QPainter>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QTransform>
#include "core/utils/Logger.h"
#include "core/document/MathSpan.h"
#include "graphics/formula/MathItem.h"
#include "graphics/factory/MathItemFactory.h"
#include "graphics/formula/MathCursor.h"
#include "graphics/formula/RowContainerItem.h"
#include "graphics/formula/NumberItem.h"
#include "graphics/formula/FractionItem.h"
#include "core/document/math/NumberMathSpan.h"
#include "graphics/scene/DocumentScene.h"

namespace QtWordEditor {

TextBlockItem::TextBlockItem(ParagraphBlock *block, QGraphicsItem *parent)
    : BaseBlockItem(block, parent),
      m_layoutEngine(new TextBlockLayoutEngine()),
      m_textWidth(Constants::PAGE_WIDTH - 2 * Constants::PAGE_MARGIN),
      m_leftIndent(0),
      m_rightIndent(0),
      m_boundingRect(0, 0, m_textWidth, 0),
      m_selectionStartOffset(-1),
      m_selectionEndOffset(-1),
      m_inMathEditMode(false),
      m_rootMathItem(nullptr),
      m_mathCursor(nullptr),
      m_clickedMathItem(nullptr),
      m_clickedRegion(-1),
      m_clickedLocalPos(0, 0),
      m_useUnifiedCursor(true)  // 默认使用统一光标，只显示一个光标
{
    setFlag(QGraphicsItem::ItemIsSelectable, false);
    setFlag(QGraphicsItem::ItemIsFocusable, true);  // 允许接收焦点
    applyParagraphIndent();
    performLayout();
}

TextBlockItem::~TextBlockItem()
{
    delete m_layoutEngine;
    if (m_mathCursor) {
        delete m_mathCursor;
    }
}

QRectF TextBlockItem::boundingRect() const
{
    return m_boundingRect;
}

void TextBlockItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    
    // 获取 InlineSpan 列表
    QList<InlineSpan*> spans = getSpans();
    if (spans.isEmpty()) {
        return;
    }
    
    const QList<TextBlockLayoutEngine::LayoutItem> &items = m_layoutEngine->layoutItems();
    
    painter->save();
    
    // 设置高质量的渲染提示，让文字更清晰
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setRenderHint(QPainter::TextAntialiasing, true);
    painter->setRenderHint(QPainter::SmoothPixmapTransform, true);
    
    painter->translate(m_leftIndent, 0);  // 加上左缩进
    
    // 绘制每个 item 的文字，使用QTextLayout绘制，这样文字更清晰
    for (const TextBlockLayoutEngine::LayoutItem &item : items) {
        QTextLayout textLayout(item.text, item.font);
        textLayout.setCacheEnabled(true);
        
        // 开始布局
        textLayout.beginLayout();
        QTextLine textLine = textLayout.createLine();
        textLine.setLineWidth(item.width);
        textLayout.endLayout();
        
        // 设置画笔颜色
        painter->setPen(item.style.textColor());
        
        // 绘制文字
        textLine.draw(painter, QPointF(item.position.x(), item.position.y()));
    }
    
    painter->restore();
}

void TextBlockItem::updateGeometry()
{
    performLayout();
}

void TextBlockItem::updateBlock()
{
    ParagraphBlock *para = qobject_cast<ParagraphBlock*>(m_block);
    if (!para)
        return;
    
    // 清除旧的 MathItem
    clearMathItems();
    
    applyParagraphIndent();
    
    // ========== 第一步：先创建所有 MathItem，获取真实尺寸 ==========
    QList<InlineSpan*> spans = getSpans();
    QHash<InlineSpan*, MathItem*> mathItemMap;
    QHash<InlineSpan*, QSizeF> mathSizeMap;
    QHash<InlineSpan*, qreal> mathBaselineMap;
    
    for (InlineSpan *span : spans) {
        if (span->type() == InlineSpan::Math) {
            MathSpan *mathSpan = static_cast<MathSpan*>(span);
            MathItem *mathItem = MathItemFactory::createMathItem(mathSpan);
            if (mathItem) {
                mathItem->setParentItem(this);
                mathItem->setVisible(false); // 先隐藏，等布局后再显示
                m_mathItems.append(mathItem);
                mathItemMap.insert(span, mathItem);
                mathSizeMap.insert(span, mathItem->boundingRect().size());
                mathBaselineMap.insert(span, mathItem->baseline());
                qDebug() << "  预创建 MathItem，span=" << span 
                         << "size=" << mathItem->boundingRect().size()
                         << "baseline=" << mathItem->baseline();
            }
        }
    }
    // ===============================================================
    
    // ========== 第二步：执行布局（现在可以使用真实尺寸） ==========
    // 修改：在创建LayoutItem时，如果是MathSpan，使用真实尺寸
    performLayoutWithMathSizes(mathSizeMap, mathBaselineMap);
    // ===============================================================
    
    // ========== 调试信息 ==========
    const QList<TextBlockLayoutEngine::LayoutItem> &items = m_layoutEngine->layoutItems();
    qDebug() << "TextBlockItem::updateBlock() - items 数量:" << items.size();
    for (int i = 0; i < items.size(); ++i) {
        const TextBlockLayoutEngine::LayoutItem &item = items[i];
        qDebug() << "  item" << i << ":"
                 << "inlineSpan=" << item.inlineSpan
                 << "type=" << (item.inlineSpan ? (int)item.inlineSpan->type() : -1)
                 << "position=" << item.position
                 << "width=" << item.width
                 << "height=" << item.height;
    }
    // ===============================
    
    // ========== 第三步：设置 MathItem 的位置并显示 ==========
    for (const TextBlockLayoutEngine::LayoutItem &item : items) {
        if (item.inlineSpan && item.inlineSpan->type() == InlineSpan::Math) {
            MathItem *mathItem = mathItemMap.value(item.inlineSpan, nullptr);
            if (mathItem) {
                mathItem->setPos(item.position + QPointF(m_leftIndent, 0));
                mathItem->setVisible(true);
                qDebug() << "  设置 MathItem 位置，span=" << item.inlineSpan 
                         << "pos=" << item.position + QPointF(m_leftIndent, 0);
            }
        }
    }
    // ===============================================================
    
    qDebug() << "TextBlockItem::updateBlock() - m_mathItems 数量:" << m_mathItems.size();
    
    update(); // 触发重绘，确保内容更新后立即显示
}

void TextBlockItem::clearMathItems()
{
    qDeleteAll(m_mathItems);
    m_mathItems.clear();
}

QList<InlineSpan*> TextBlockItem::getSpans() const
{
    QList<InlineSpan*> spans;
    ParagraphBlock *para = qobject_cast<ParagraphBlock*>(m_block);
    if (para) {
        for (int i = 0; i < para->inlineSpanCount(); ++i) {
            spans << para->inlineSpan(i);
        }
    }
    return spans;
}

void TextBlockItem::performLayout()
{
    QList<InlineSpan*> spans = getSpans();
    if (spans.isEmpty()) {
        m_boundingRect = QRectF(0, 0, m_textWidth, 0);
        return;
    }
    
    ParagraphBlock *para = qobject_cast<ParagraphBlock*>(m_block);
    if (para) {
        m_layoutEngine->setParagraphStyle(para->paragraphStyle());
    }
    
    // 计算可用宽度（减去缩进）
    qreal availableWidth = m_textWidth - m_leftIndent - m_rightIndent;
    if (availableWidth < 10.0) {
        availableWidth = 10.0;
    }
    
    m_layoutEngine->setAvailableWidth(availableWidth);
    m_layoutEngine->setWrapMode(TextBlockLayoutEngine::WrapMode::WrapAnywhere);
    
    // 执行布局
    m_layoutEngine->layout(spans);
    
    // 更新边界矩形（加上左缩进）
    m_boundingRect = QRectF(0, 0, m_textWidth, m_layoutEngine->totalHeight());
}

void TextBlockItem::performLayoutWithMathSizes(const QHash<InlineSpan*, QSizeF> &mathSizeMap, 
                                                  const QHash<InlineSpan*, qreal> &mathBaselineMap)
{
    QList<InlineSpan*> spans = getSpans();
    if (spans.isEmpty()) {
        m_boundingRect = QRectF(0, 0, m_textWidth, 0);
        return;
    }
    
    ParagraphBlock *para = qobject_cast<ParagraphBlock*>(m_block);
    if (para) {
        m_layoutEngine->setParagraphStyle(para->paragraphStyle());
    }
    
    // 计算可用宽度（减去缩进）
    qreal availableWidth = m_textWidth - m_leftIndent - m_rightIndent;
    if (availableWidth < 10.0) {
        availableWidth = 10.0;
    }
    
    m_layoutEngine->setAvailableWidth(availableWidth);
    m_layoutEngine->setWrapMode(TextBlockLayoutEngine::WrapMode::WrapAnywhere);
    
    // 执行布局（使用MathSpan的真实尺寸）
    m_layoutEngine->layout(spans, mathSizeMap, mathBaselineMap);
    
    // 更新边界矩形（加上左缩进）
    m_boundingRect = QRectF(0, 0, m_textWidth, m_layoutEngine->totalHeight());
}

void TextBlockItem::applyParagraphIndent()
{
    ParagraphBlock *para = qobject_cast<ParagraphBlock*>(m_block);
    if (!para) {
        return;
    }
    
    // 获取段落样式中的缩进值
    ParagraphStyle style = para->paragraphStyle();
    m_leftIndent = style.leftIndent();
    m_rightIndent = style.rightIndent();
}

int TextBlockItem::hitTest(const QPointF &localPos) const
{
    QList<InlineSpan*> spans = getSpans();
    if (spans.isEmpty()) {
        return 0;
    }
    
    // 调整坐标（减去左缩进）
    QPointF adjustedPos = localPos - QPointF(m_leftIndent, 0);
    
    TextBlockLayoutEngine::CursorHitResult result = m_layoutEngine->hitTest(adjustedPos, spans);
    return result.globalOffset;
}

TextBlockItem::CursorVisualInfo TextBlockItem::cursorPositionAt(int globalOffset) const
{
    CursorVisualInfo result;
    result.position = QPointF(0, 0);
    result.height = 20;
    
    QList<InlineSpan*> spans = getSpans();
    if (spans.isEmpty()) {
        return result;
    }
    
    TextBlockLayoutEngine::CursorVisualResult visualResult = m_layoutEngine->cursorPositionAt(globalOffset, spans);
    
    // 加上左缩进
    result.position = visualResult.position + QPointF(m_leftIndent, 0);
    result.height = visualResult.height;
    
    return result;
}

QList<QRectF> TextBlockItem::selectionRects(int startOffset, int endOffset) const
{
    QList<QRectF> rects;
    
    QList<InlineSpan*> spans = getSpans();
    if (spans.isEmpty()) {
        return rects;
    }
    
    // 使用 TextBlockLayoutEngine 的新方法，按行计算选择矩形，确保背景色连续
    QList<QRectF> engineRects = m_layoutEngine->selectionRects(startOffset, endOffset, spans);
    
    // 加上左缩进
    for (const QRectF &rect : engineRects) {
        QRectF adjustedRect = rect;
        adjustedRect.translate(m_leftIndent, 0);
        rects << adjustedRect;
    }
    
    return rects;
}

QString TextBlockItem::toPlainText() const
{
    QString text;
    QList<InlineSpan*> spans = getSpans();
    for (const InlineSpan *span : spans) {
        if (span->type() == InlineSpan::Text) {
            const TextSpan *textSpan = static_cast<const TextSpan*>(span);
            text += textSpan->text();
        }
    }
    return text;
}

void TextBlockItem::setTextWidth(qreal width)
{
    if (m_textWidth != width) {
        m_textWidth = width;
        performLayout();
        update();
    }
}

void TextBlockItem::setSelectionRange(int startOffset, int endOffset)
{
    if (m_selectionStartOffset != startOffset || m_selectionEndOffset != endOffset) {
        m_selectionStartOffset = startOffset;
        m_selectionEndOffset = endOffset;
        update();
    }
}

void TextBlockItem::clearSelection()
{
    setSelectionRange(-1, -1);
}

QGraphicsTextItem *TextBlockItem::textItem() const
{
    // 为了兼容性保留，返回nullptr
    return nullptr;
}

void TextBlockItem::setFont(const QFont &font)
{
    // 为了兼容性保留，不做任何操作
    Q_UNUSED(font);
}

QFont TextBlockItem::font() const
{
    // 为了兼容性保留，返回默认字体
    return QFont();
}

void TextBlockItem::setPlainText(const QString &text)
{
    // 为了兼容性保留，不做任何操作
    Q_UNUSED(text);
}

qreal TextBlockItem::textWidth() const
{
    return m_textWidth;
}

// ========== 公式编辑模式 ==========

bool TextBlockItem::isInMathEditMode() const
{
    return m_inMathEditMode;
}

void TextBlockItem::enterMathEditMode(MathSpan *mathSpan)
{
    if (m_inMathEditMode) {
        return;
    }
    
    m_inMathEditMode = true;
    
    // ========== 严格隐藏 DocumentScene 的普通光标 ==========
    QGraphicsScene *graphicsScene = this->scene();
    DocumentScene *scene = dynamic_cast<DocumentScene *>(graphicsScene);
    if (scene) {
        scene->setCursorVisible(false);
    }
    
    // 创建 MathCursor（保持兼容性）
    if (!m_mathCursor) {
        m_mathCursor = new MathCursor(this);
    }
    // 确保 MathCursor 可见
    m_mathCursor->setVisible(true);
    
    // 查找对应的 MathItem
    m_rootMathItem = nullptr;
    RowContainerItem *rootContainer = nullptr;
    for (QGraphicsItem *item : m_mathItems) {
        MathItem *mathItem = dynamic_cast<MathItem*>(item);
        if (mathItem && mathItem->mathSpan() == mathSpan) {
            m_rootMathItem = mathItem;
            // 检查是否是 RowContainerItem
            rootContainer = dynamic_cast<RowContainerItem*>(mathItem);
            break;
        }
    }
    
    // ========== 检查是否是 FractionItem 并且有点击信息 ==========
    FractionItem *fracItem = dynamic_cast<FractionItem*>(m_clickedMathItem);
    if (fracItem && m_clickedRegion >= 0 && m_clickedRegion <= 1) {
        qDebug() << "[enterMathEditMode] 是 FractionItem，点击区域=" << m_clickedRegion;
        
        NumberItem *targetNumberItem = nullptr;
        QPointF numberLocalPos(0, 0);
        
        // 获取分子或分母的 NumberItem
        if (m_clickedRegion == 0) {
            // 分子
            MathItem *numItem = fracItem->numeratorItem();
            targetNumberItem = dynamic_cast<NumberItem*>(numItem);
            if (targetNumberItem) {
                // 计算相对于 NumberItem 的坐标
                QPointF numLocalPos = m_clickedLocalPos - fracItem->numeratorPos();
                numberLocalPos = numLocalPos;
            }
        } else if (m_clickedRegion == 1) {
            // 分母
            MathItem *denItem = fracItem->denominatorItem();
            targetNumberItem = dynamic_cast<NumberItem*>(denItem);
            if (targetNumberItem) {
                // 计算相对于 NumberItem 的坐标
                QPointF denLocalPos = m_clickedLocalPos - fracItem->denominatorPos();
                numberLocalPos = denLocalPos;
            }
        }
        
        // 如果找到了目标 NumberItem，设置光标到该位置
        if (targetNumberItem) {
            qDebug() << "[enterMathEditMode] 找到目标 NumberItem=" << targetNumberItem;
            
            // 使用 NumberItem 的 hitTestX 方法获取字符位置
            int charPosition = targetNumberItem->hitTestX(numberLocalPos.x());
            qDebug() << "[enterMathEditMode] 字符位置=" << charPosition;
            
            m_mathCursor->setHeight(targetNumberItem->boundingRect().height());
            m_mathCursor->setPosition(targetNumberItem, charPosition);
            
            setFocus();
            qDebug() << "进入公式编辑模式（分数的 NumberItem）";
            return;
        }
    }
    
    // ========== 默认情况（不是 FractionItem 或没有点击信息） ==========
    qDebug() << "[enterMathEditMode] 默认情况处理";
    
    // 如果找到了根 MathItem，并且它是容器，设置 MathCursor
    if (rootContainer) {
        m_mathCursor->setHeight(rootContainer->boundingRect().height());
        m_mathCursor->setPosition(rootContainer, 0);
        // 将 MathCursor 设置为 rootContainer 的子项，这样坐标是相对的
        m_mathCursor->setParentItem(rootContainer);
    } else if (m_rootMathItem) {
        // 如果不是容器，也设置高度
        m_mathCursor->setHeight(m_rootMathItem->boundingRect().height());
        m_mathCursor->setParentItem(m_rootMathItem);
    }
    
    // 设置焦点
    setFocus();
    
    qDebug() << "进入公式编辑模式";
}

void TextBlockItem::exitMathEditMode()
{
    if (!m_inMathEditMode) {
        return;
    }
    
    m_inMathEditMode = false;
    
    // ========== 彻底隐藏 MathCursor（确保完全不可见）==========
    if (m_mathCursor) {
        m_mathCursor->setVisible(false);
    }
    
    m_rootMathItem = nullptr;
    
    // ========== 严格重新显示 DocumentScene 的统一光标 ==========
    QGraphicsScene *graphicsScene = this->scene();
    DocumentScene *scene = dynamic_cast<DocumentScene *>(graphicsScene);
    if (scene) {
        scene->setCursorVisible(true);
    }
    
    qDebug() << "退出公式编辑模式";
}

MathCursor *TextBlockItem::mathCursor() const
{
    return m_mathCursor;
}

void TextBlockItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    qDebug() << "[TextBlockItem::mousePressEvent] 开始, scenePos=" << event->scenePos() << "button=" << event->button();
    
    // ========== 只在左键时才处理公式编辑逻辑，右键不改变光标位置 ==========
    if (event->button() != Qt::LeftButton) {
        // 右键点击：完全不处理任何光标逻辑，直接返回
        return;
    }
    
    // 重置点击信息
    m_clickedMathItem = nullptr;
    m_clickedRegion = -1;
    m_clickedLocalPos = QPointF(0, 0);
    
    // 检查是否点击了 MathItem（包括子项）
    MathItem *clickedMathItem = nullptr;
    
    // 遍历所有子项，查找被点击的 MathItem
    for (QGraphicsItem *child : childItems()) {
        QPointF localPos = child->mapFromScene(event->scenePos());
        if (child->contains(localPos)) {
            qDebug() << "  找到子项: " << child << ", localPos=" << localPos;
            
            // 检查是否是 MathItem
            MathItem *mathItem = dynamic_cast<MathItem*>(child);
            if (mathItem) {
                clickedMathItem = mathItem;
                qDebug() << "  找到 MathItem: " << clickedMathItem;
                break;
            }
            
            // 如果不是 MathItem，检查它的父项是否是 MathItem
            QGraphicsItem *parent = child->parentItem();
            while (parent) {
                MathItem *parentMathItem = dynamic_cast<MathItem*>(parent);
                if (parentMathItem) {
                    clickedMathItem = parentMathItem;
                    qDebug() << "  找到父 MathItem: " << clickedMathItem;
                    break;
                }
                parent = parent->parentItem();
            }
            if (clickedMathItem) {
                break;
            }
        }
    }
    
    if (clickedMathItem) {
        qDebug() << "  准备进入公式编辑模式, span=" << clickedMathItem->mathSpan();
        
        // 转换到 clickedMathItem 的局部坐标
        QPointF mathLocalPos = clickedMathItem->mapFromScene(event->scenePos());
        qDebug() << "  mathLocalPos=" << mathLocalPos;
        
        // 检查是否是 FractionItem，判断是分子还是分母
        int region = clickedMathItem->hitTestRegion(mathLocalPos);
        qDebug() << "  点击区域: " << region;
        
        // 存储点击信息
        m_clickedMathItem = clickedMathItem;
        m_clickedRegion = region;
        m_clickedLocalPos = mathLocalPos;
        
        // 进入公式编辑模式
        enterMathEditMode(clickedMathItem->mathSpan());
        
        // 调用 MathItem 的鼠标事件
        clickedMathItem->mousePressEvent(event);
        return;
    }
    
    qDebug() << "  没有点击 MathItem";
    
    // 如果不是点击 MathItem，退出公式编辑模式
    if (m_inMathEditMode) {
        exitMathEditMode();
    }
    
    QGraphicsItem::mousePressEvent(event);
}

void TextBlockItem::keyPressEvent(QKeyEvent *event)
{
    if (m_inMathEditMode && m_mathCursor) {
        qDebug() << "[TextBlockItem::keyPressEvent] 公式编辑模式，key=" << event->key() << ", text=" << event->text();
        
        // 检查是否在 NumberItem 内部（数字模式）
        if (m_mathCursor->cursorMode() == MathCursor::NumberMode) {
            NumberItem *numberItem = m_mathCursor->currentNumberItem();
            if (numberItem) {
                NumberMathSpan *numSpan = numberItem->numberSpan();
                if (numSpan) {
                    QString text = numSpan->text();
                    int pos = m_mathCursor->position();
                    
                    // 处理可打印字符
                    if (event->text().length() > 0 && event->text()[0].isPrint()) {
                        // 插入字符
                        text.insert(pos, event->text());
                        numSpan->setText(text);
                        
                        // 更新布局
                        numberItem->updateLayout();
                        
                        // 光标向右移动
                        m_mathCursor->setPosition(numberItem, pos + 1);
                        return;
                    }
                    
                    // 处理退格键
                    if (event->key() == Qt::Key_Backspace) {
                        if (pos > 0) {
                            text.remove(pos - 1, 1);
                            numSpan->setText(text);
                            
                            // 更新布局
                            numberItem->updateLayout();
                            
                            // 光标向左移动
                            m_mathCursor->setPosition(numberItem, pos - 1);
                        }
                        return;
                    }
                    
                    // 处理 Delete 键
                    if (event->key() == Qt::Key_Delete) {
                        if (pos < text.length()) {
                            text.remove(pos, 1);
                            numSpan->setText(text);
                            
                            // 更新布局
                            numberItem->updateLayout();
                        }
                        return;
                    }
                }
            }
        }
        
        // 处理导航键
        switch (event->key()) {
            case Qt::Key_Left:
                m_mathCursor->moveLeft();
                break;
            case Qt::Key_Right:
                m_mathCursor->moveRight();
                break;
            case Qt::Key_Up:
                m_mathCursor->moveUp();
                break;
            case Qt::Key_Down:
                m_mathCursor->moveDown();
                break;
            case Qt::Key_Escape:
                exitMathEditMode();
                break;
            default:
                QGraphicsItem::keyPressEvent(event);
                break;
        }
        return;
    }
    
    QGraphicsItem::keyPressEvent(event);
}

// ========== 统一光标（新）相关方法实现 ==========

void TextBlockItem::setUseUnifiedCursor(bool useUnified)
{
    m_useUnifiedCursor = useUnified;
}

bool TextBlockItem::useUnifiedCursor() const
{
    return m_useUnifiedCursor;
}

} // namespace QtWordEditor
