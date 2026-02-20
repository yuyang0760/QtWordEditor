#include "graphics/items/TextBlockItem.h"
#include "core/document/ParagraphBlock.h"
#include "graphics/items/TextBlockLayoutEngine.h"
#include "core/utils/Constants.h"
#include <QPainter>
#include <QDebug>
#include "core/utils/Logger.h"

namespace QtWordEditor {

TextBlockItem::TextBlockItem(ParagraphBlock *block, QGraphicsItem *parent)
    : BaseBlockItem(block, parent),
      m_layoutEngine(new TextBlockLayoutEngine()),
      m_textWidth(Constants::PAGE_WIDTH - 2 * Constants::PAGE_MARGIN),
      m_leftIndent(0),
      m_rightIndent(0),
      m_boundingRect(0, 0, m_textWidth, 0),
      m_selectionStartOffset(-1),
      m_selectionEndOffset(-1)
{
    setFlag(QGraphicsItem::ItemIsSelectable, false);
    applyParagraphIndent();
    performLayout();
}

TextBlockItem::~TextBlockItem()
{
    delete m_layoutEngine;
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
    
    applyParagraphIndent();
    performLayout();
    update(); // 触发重绘，确保内容更新后立即显示
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

QString TextBlockItem::toPlainText() const
{
    QString text;
    QList<Span> spans = getSpans();
    for (const Span &span : spans) {
        text += span.text();
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

} // namespace QtWordEditor
