#include "graphics/items/TextBlockItem.h"
#include "core/document/ParagraphBlock.h"
#include "core/utils/Constants.h"
#include "core/utils/Logger.h"
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QTextLayout>
#include <QTextLine>

namespace QtWordEditor {

TextBlockItem::TextBlockItem(ParagraphBlock *block, QGraphicsItem *parent)
    : BaseBlockItem(block, parent)
    , m_layoutEngine(new TextBlockLayoutEngine())
    , m_textWidth(800)
    , m_leftIndent(0)
    , m_rightIndent(0)
{
    setFlag(QGraphicsItem::ItemIsSelectable, false);
    setFlag(QGraphicsItem::ItemIsFocusable, false);
    
    if (block) {
        applyParagraphIndent();
        performLayout();
    }
}

TextBlockItem::~TextBlockItem()
{
    delete m_layoutEngine;
}

void TextBlockItem::setTextWidth(qreal width)
{
    if (!qFuzzyCompare(m_textWidth, width)) {
        m_textWidth = width;
        performLayout();
        updateGeometry();
    }
}

qreal TextBlockItem::textWidth() const
{
    return m_textWidth;
}

QGraphicsTextItem *TextBlockItem::textItem() const
{
    // 为了兼容性保留，返回 nullptr
    return nullptr;
}

void TextBlockItem::setFont(const QFont &font)
{
    Q_UNUSED(font);
    // 不再单独设置字体，字体由 Span 控制
}

QFont TextBlockItem::font() const
{
    // 返回默认字体
    QFont font;
    font.setPointSize(Constants::DefaultFontSize);
    return font;
}

void TextBlockItem::setPlainText(const QString &text)
{
    Q_UNUSED(text);
    // 不再支持直接设置纯文本，必须通过 ParagraphBlock 操作
}

QString TextBlockItem::toPlainText() const
{
    QList<Span> spans = getSpans();
    QString result;
    for (const Span &span : spans) {
        result += span.text();
    }
    return result;
}

QRectF TextBlockItem::boundingRect() const
{
    return m_boundingRect;
}

void TextBlockItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    
    // 获取 Span 列表
    QList<Span> spans = getSpans();
    if (spans.isEmpty()) {
        return;
    }
    
    // 用于测试的背景颜色：红、绿、蓝循环
    QList<QColor> bgColors;
    bgColors << QColor(255, 200, 200, 100);  // 红色半透明
    bgColors << QColor(200, 255, 200, 100);  // 绿色半透明
    bgColors << QColor(200, 200, 255, 100);  // 蓝色半透明
    
    const QList<TextBlockLayoutEngine::LayoutItem> &items = m_layoutEngine->layoutItems();
    const QList<TextBlockLayoutEngine::LineInfo> &lines = m_layoutEngine->lines();
    
    painter->save();
    painter->translate(m_leftIndent, 0);  // 加上左缩进
    
    // 第一步：先绘制每个 item 的背景色（按 spanIndex 区分）
    for (const TextBlockLayoutEngine::LayoutItem &item : items) {
        QColor bgColor = bgColors[item.spanIndex % bgColors.size()];
        painter->fillRect(QRectF(item.position.x(), item.position.y(), 
                                  item.width, item.height), bgColor);
    }
    
    // 第二步：绘制每个 item 的文字
    for (const TextBlockLayoutEngine::LayoutItem &item : items) {
        painter->setFont(item.font);
        painter->setPen(item.style.textColor());
        
        // 计算基线位置
        qreal baselineY = item.position.y() + item.ascent;
        
        painter->drawText(QPointF(item.position.x(), baselineY), item.text);
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
    
    applyParagraphIndent();
    performLayout();
}

QList<Span> TextBlockItem::getSpans() const
{
    QList<Span> spans;
    ParagraphBlock *para = qobject_cast<ParagraphBlock*>(m_block);
    if (para) {
        for (int i = 0; i < para->spanCount(); ++i) {
            spans << para->span(i);
        }
    }
    return spans;
}

void TextBlockItem::performLayout()
{
    QList<Span> spans = getSpans();
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
    QList<Span> spans = getSpans();
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
    
    QList<Span> spans = getSpans();
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
    
    QList<Span> spans = getSpans();
    if (spans.isEmpty()) {
        return rects;
    }
    
    const QList<TextBlockLayoutEngine::LayoutItem> &items = m_layoutEngine->layoutItems();
    
    // 归一化偏移
    int normalizedStart = qMin(startOffset, endOffset);
    int normalizedEnd = qMax(startOffset, endOffset);
    
    LOG_DEBUG(QString("TextBlockItem::selectionRects start=%1, end=%2").arg(normalizedStart).arg(normalizedEnd));
    
    if (normalizedStart >= normalizedEnd) {
        return rects;
    }
    
    // 遍历所有 item，找到重叠的部分
    for (const TextBlockLayoutEngine::LayoutItem &item : items) {
        LOG_DEBUG(QString("  检查 item: spanIndex=%1, globalStart=%2, globalEnd=%3, text=%4")
                    .arg(item.spanIndex).arg(item.globalStartOffset).arg(item.globalEndOffset).arg(item.text));
        
        // 检查是否与选择范围重叠
        if (item.globalEndOffset <= normalizedStart || item.globalStartOffset >= normalizedEnd) {
            continue;
        }
        
        // 计算在这个 item 内的选择范围
        int selStartInItem = qMax(normalizedStart, item.globalStartOffset) - item.globalStartOffset;
        int selEndInItem = qMin(normalizedEnd, item.globalEndOffset) - item.globalStartOffset;
        
        LOG_DEBUG(QString("    重叠! selStartInItem=%1, selEndInItem=%2, item.text.length()=%3")
                    .arg(selStartInItem).arg(selEndInItem).arg(item.text.length()));
        
        // 直接使用这个 item 的文本片段来计算选择范围
        QTextLayout textLayout(item.text, item.font);
        textLayout.beginLayout();
        QTextLine textLine = textLayout.createLine();
        textLine.setLineWidth(item.width);  // 这个 item 的宽度已经确定了！
        textLayout.endLayout();
        
        qreal startX = textLine.cursorToX(selStartInItem);
        qreal endX = textLine.cursorToX(selEndInItem);
        
        LOG_DEBUG(QString("    startX=%1, endX=%2, width=%3").arg(startX).arg(endX).arg(endX - startX));
        
        // 创建选择矩形（加上左缩进）
        QRectF rect(
            m_leftIndent + item.position.x() + startX,
            item.position.y(),
            endX - startX,
            item.height
        );
        LOG_DEBUG(QString("    添加矩形: x=%1, y=%2, w=%3, h=%4")
                    .arg(rect.x()).arg(rect.y()).arg(rect.width()).arg(rect.height()));
        rects << rect;
    }
    
    return rects;
}

} // namespace QtWordEditor
