#include "graphics/items/TextBlockItem.h"
#include "core/document/ParagraphBlock.h"
#include "core/document/ParagraphStyle.h"
#include "core/document/CharacterStyle.h"
#include "core/utils/Constants.h"
#include "core/utils/Logger.h"
#include "graphics/items/TextBlockLayoutEngine.h"
#include <QFont>
#include <QFontMetrics>
#include <QPainter>
#include <QTextLayout>
#include <QTextLine>

namespace QtWordEditor {

TextBlockItem::TextBlockItem(ParagraphBlock *block, QGraphicsItem *parent)
    : BaseBlockItem(block, parent)
    , m_layoutEngine(new TextBlockLayoutEngine())
    , m_textWidth(Constants::PAGE_WIDTH - 2 * Constants::PAGE_MARGIN)
    , m_leftIndent(0)
    , m_rightIndent(0)
    , m_boundingRect(0, 0, 0, 0)
{
    setFlag(QGraphicsItem::ItemIsSelectable, false);
    setFlag(QGraphicsItem::ItemIsFocusable, false);
    
    // 从 ParagraphBlock 读取初始内容并布局
    if (block) {
        applyParagraphIndent();
        performLayout();
    }
}

TextBlockItem::~TextBlockItem()
{
    delete m_layoutEngine;
}

QGraphicsTextItem *TextBlockItem::textItem() const
{
    // 为了兼容性保留此接口，但返回 nullptr，因为不再使用 QGraphicsTextItem
    return nullptr;
}

void TextBlockItem::setTextWidth(qreal width)
{
    if (m_textWidth != width) {
        m_textWidth = width;
        performLayout();
    }
}

qreal TextBlockItem::textWidth() const
{
    return m_textWidth;
}

void TextBlockItem::setFont(const QFont &font)
{
    Q_UNUSED(font);
    // 此方法为了兼容性保留，但不再使用
    // 字体现在由 CharacterStyle 管理
}

QFont TextBlockItem::font() const
{
    // 返回默认字体，为了兼容性
    QFont font;
    font.setPointSize(Constants::DefaultFontSize);
    return font;
}

void TextBlockItem::setPlainText(const QString &text)
{
    Q_UNUSED(text);
    // 此方法为了兼容性保留
    // 文本内容现在通过 ParagraphBlock 管理
}

QString TextBlockItem::toPlainText() const
{
    QString result;
    QList<Span> spans = getSpans();
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
    
    // 简单方案：把所有 Span 的文本拼起来
    QString fullText;
    for (const Span &span : spans) {
        fullText += span.text();
    }
    
    if (fullText.isEmpty()) {
        return;
    }
    
    // 使用第一个 Span 的样式（简化版）
    const Span &firstSpan = spans.first();
    
    // 创建字体
    QFont font;
    if (!firstSpan.style().fontFamily().isEmpty()) {
        font.setFamily(firstSpan.style().fontFamily());
    }
    if (firstSpan.style().fontSize() > 0) {
        font.setPointSize(firstSpan.style().fontSize());
    } else {
        font.setPointSize(Constants::DefaultFontSize);
    }
    font.setBold(firstSpan.style().bold());
    font.setItalic(firstSpan.style().italic());
    font.setUnderline(firstSpan.style().underline());
    font.setStrikeOut(firstSpan.style().strikeOut());
    
    // 使用 QTextLayout 绘制
    QTextLayout textLayout(fullText, font);
    textLayout.beginLayout();
    
    qreal availableWidth = m_textWidth - m_leftIndent - m_rightIndent;
    if (availableWidth < 10.0) {
        availableWidth = 10.0;
    }
    
    qreal currentY = 0;
    while (true) {
        QTextLine line = textLayout.createLine();
        if (!line.isValid()) break;
        
        line.setLineWidth(availableWidth);
        line.setPosition(QPointF(0, currentY));
        currentY += line.height();
    }
    textLayout.endLayout();
    
    // 绘制
    painter->save();
    painter->translate(m_leftIndent, 0);  // 加上左缩进
    painter->setFont(font);
    painter->setPen(firstSpan.style().textColor());
    textLayout.draw(painter, QPointF(0, 0));
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
    
    // 更新边界矩形
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
    // 调整坐标（减去左缩进）
    QPointF adjustedPos = localPos - QPointF(m_leftIndent, 0);
    
    QList<Span> spans = getSpans();
    TextBlockLayoutEngine::CursorHitResult result = m_layoutEngine->hitTest(adjustedPos, spans);
    
    return result.globalOffset;
}

TextBlockItem::CursorVisualInfo TextBlockItem::cursorPositionAt(int globalOffset) const
{
    QList<Span> spans = getSpans();
    TextBlockLayoutEngine::CursorVisualResult visualResult = 
        m_layoutEngine->cursorPositionAt(globalOffset, spans);
    
    TextBlockItem::CursorVisualInfo result;
    result.position = visualResult.position + QPointF(m_leftIndent, 0);  // 加上左缩进
    result.height = visualResult.height;
    
    return result;
}

} // namespace QtWordEditor
