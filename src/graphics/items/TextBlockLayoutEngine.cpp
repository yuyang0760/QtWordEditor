#include "graphics/items/TextBlockLayoutEngine.h"
#include <QTextLayout>
#include <QTextLine>
#include <limits>

namespace QtWordEditor {

TextBlockLayoutEngine::TextBlockLayoutEngine()
    : m_availableWidth(0)
    , m_wrapMode(WrapMode::WrapAnywhere)
    , m_totalWidth(0)
    , m_totalHeight(0)
{
}

TextBlockLayoutEngine::~TextBlockLayoutEngine()
{
}

void TextBlockLayoutEngine::setAvailableWidth(qreal width)
{
    m_availableWidth = width;
}

void TextBlockLayoutEngine::setParagraphStyle(const ParagraphStyle &style)
{
    m_paragraphStyle = style;
}

void TextBlockLayoutEngine::setWrapMode(WrapMode mode)
{
    m_wrapMode = mode;
}

void TextBlockLayoutEngine::layout(const QList<Span> &spans)
{
    // 清除之前的布局结果
    clear();
    
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
    QFont font = createFontFromStyle(firstSpan.style());
    
    // 使用 QTextLayout 进行布局
    QTextLayout textLayout(fullText, font);
    textLayout.beginLayout();
    
    qreal currentY = 0;
    QList<QTextLine> textLines;
    
    while (true) {
        QTextLine line = textLayout.createLine();
        if (!line.isValid()) {
            break;
        }
        
        line.setLineWidth(m_availableWidth);
        line.setPosition(QPointF(0, currentY));
        textLines << line;
        
        currentY += line.height();
    }
    
    textLayout.endLayout();
    
    // 保存行信息
    m_lines.reserve(textLines.size());
    for (int i = 0; i < textLines.size(); ++i) {
        const QTextLine &textLine = textLines[i];
        LineInfo lineInfo;
        lineInfo.rect = QRectF(0, textLine.y(), textLine.naturalTextWidth(), textLine.height());
        lineInfo.maxBaseline = textLine.ascent();
        m_lines << lineInfo;
    }
    
    // 保存总尺寸
    m_totalWidth = m_availableWidth;
    m_totalHeight = currentY;
    
    // 创建一个简单的 LayoutItem（为了兼容）
    LayoutItem item;
    item.spanIndex = 0;
    item.text = fullText;
    item.style = firstSpan.style();
    item.font = font;
    item.width = m_totalWidth;
    item.height = m_totalHeight;
    item.baseline = textLines.isEmpty() ? 0 : textLines.first().ascent();
    item.position = QPointF(0, 0);
    m_layoutItems << item;
}

const QList<TextBlockLayoutEngine::LayoutItem> &TextBlockLayoutEngine::layoutItems() const
{
    return m_layoutItems;
}

const QList<TextBlockLayoutEngine::LineInfo> &TextBlockLayoutEngine::lines() const
{
    return m_lines;
}

qreal TextBlockLayoutEngine::totalWidth() const
{
    return m_totalWidth;
}

qreal TextBlockLayoutEngine::totalHeight() const
{
    return m_totalHeight;
}

TextBlockLayoutEngine::CursorHitResult TextBlockLayoutEngine::hitTest(const QPointF &localPos, const QList<Span> &spans) const
{
    CursorHitResult result;
    result.globalOffset = 0;
    result.lineIndex = -1;
    result.itemIndex = 0;
    result.offsetInItem = 0;
    
    if (spans.isEmpty() || m_layoutItems.isEmpty()) {
        return result;
    }
    
    const LayoutItem &item = m_layoutItems.first();
    
    // 使用 QTextLayout 来计算
    QTextLayout textLayout(item.text, item.font);
    textLayout.beginLayout();
    
    qreal currentY = 0;
    QList<QTextLine> textLines;
    
    while (true) {
        QTextLine line = textLayout.createLine();
        if (!line.isValid()) {
            break;
        }
        
        line.setLineWidth(m_availableWidth);
        line.setPosition(QPointF(0, currentY));
        textLines << line;
        
        currentY += line.height();
    }
    
    textLayout.endLayout();
    
    if (textLines.isEmpty()) {
        return result;
    }
    
    // 找到点击的行
    int hitLineIndex = 0;
    for (int i = 0; i < textLines.size(); ++i) {
        const QTextLine &line = textLines[i];
        qreal lineTop = line.y();
        qreal lineBottom = lineTop + line.height();
        
        if (localPos.y() >= lineTop && localPos.y() <= lineBottom) {
            hitLineIndex = i;
            break;
        }
    }
    
    // 如果没找到，用第一个或最后一个
    if (localPos.y() < textLines.first().y()) {
        hitLineIndex = 0;
    } else if (localPos.y() > textLines.last().y() + textLines.last().height()) {
        hitLineIndex = textLines.size() - 1;
    }
    
    const QTextLine &hitLine = textLines[hitLineIndex];
    
    // 计算偏移
    int offset = hitLine.xToCursor(localPos.x());
    
    // 确保在有效范围内
    offset = qBound(0, offset, item.text.length());
    
    result.globalOffset = offset;
    result.lineIndex = hitLineIndex;
    result.itemIndex = 0;
    result.offsetInItem = offset;
    
    return result;
}

TextBlockLayoutEngine::CursorVisualResult TextBlockLayoutEngine::cursorPositionAt(int globalOffset, const QList<Span> &spans) const
{
    CursorVisualResult result;
    result.position = QPointF(0, 0);
    result.height = 20;
    
    if (spans.isEmpty() || m_layoutItems.isEmpty()) {
        return result;
    }
    
    const LayoutItem &item = m_layoutItems.first();
    
    // 使用 QTextLayout 来计算
    QTextLayout textLayout(item.text, item.font);
    textLayout.beginLayout();
    
    qreal currentY = 0;
    QList<QTextLine> textLines;
    
    while (true) {
        QTextLine line = textLayout.createLine();
        if (!line.isValid()) {
            break;
        }
        
        line.setLineWidth(m_availableWidth);
        line.setPosition(QPointF(0, currentY));
        textLines << line;
        
        currentY += line.height();
    }
    
    textLayout.endLayout();
    
    if (textLines.isEmpty()) {
        return result;
    }
    
    // 确保偏移在有效范围内
    globalOffset = qBound(0, globalOffset, item.text.length());
    
    // 找到包含这个偏移的行
    int hitLineIndex = textLines.size() - 1;
    for (int i = 0; i < textLines.size(); ++i) {
        const QTextLine &line = textLines[i];
        int lineStart = line.textStart();
        int lineEnd = lineStart + line.textLength();
        
        if (globalOffset >= lineStart && globalOffset <= lineEnd) {
            hitLineIndex = i;
            break;
        }
    }
    
    const QTextLine &hitLine = textLines[hitLineIndex];
    
    // 计算光标位置
    qreal x = hitLine.cursorToX(globalOffset);
    qreal y = hitLine.y();
    
    result.position = QPointF(x, y);
    
    // 获取光标高度
    QFontMetricsF fm(item.font);
    result.height = fm.height();
    
    return result;
}

QFont TextBlockLayoutEngine::createFontFromStyle(const CharacterStyle &style) const
{
    QFont font;
    
    if (!style.fontFamily().isEmpty()) {
        font.setFamily(style.fontFamily());
    }
    
    if (style.fontSize() > 0) {
        font.setPointSize(style.fontSize());
    }
    
    font.setBold(style.bold());
    font.setItalic(style.italic());
    font.setUnderline(style.underline());
    font.setStrikeOut(style.strikeOut());
    
    if (!qFuzzyIsNull(style.letterSpacing())) {
        font.setLetterSpacing(QFont::AbsoluteSpacing, style.letterSpacing());
    }
    
    return font;
}

TextBlockLayoutEngine::LayoutItem TextBlockLayoutEngine::calculateLayoutItem(const Span &span, qreal availableWidth) const
{
    LayoutItem item;
    item.spanIndex = -1;
    item.text = span.text();
    item.style = span.style();
    item.width = 0;
    item.height = 0;
    item.baseline = 0;
    
    item.font = createFontFromStyle(span.style());
    
    // 使用 QTextLayout 计算尺寸
    QTextLayout textLayout(item.text, item.font);
    textLayout.beginLayout();
    
    qreal currentY = 0;
    qreal maxWidth = 0;
    qreal firstLineAscent = 0;
    
    while (true) {
        QTextLine line = textLayout.createLine();
        if (!line.isValid()) {
            break;
        }
        
        line.setLineWidth(availableWidth);
        line.setPosition(QPointF(0, currentY));
        
        if (maxWidth == 0) {
            firstLineAscent = line.ascent();
        }
        
        if (line.width() > maxWidth) {
            maxWidth = line.width();
        }
        
        currentY += line.height();
    }
    
    textLayout.endLayout();
    
    // 计算尺寸
    item.width = maxWidth;
    item.height = currentY;
    item.baseline = firstLineAscent;
    
    return item;
}

void TextBlockLayoutEngine::assignItemsToLines()
{
    // 此方法暂时不需要，因为我们简化了布局逻辑
}

void TextBlockLayoutEngine::calculateLineBaselines()
{
    // 此方法暂时不需要
}

void TextBlockLayoutEngine::applyAlignment()
{
    // 此方法暂时不需要
}

void TextBlockLayoutEngine::positionItems()
{
    // 此方法暂时不需要，我们在 layout() 中直接计算位置
}

void TextBlockLayoutEngine::clear()
{
    m_layoutItems.clear();
    m_lines.clear();
    m_totalWidth = 0;
    m_totalHeight = 0;
}

} // namespace QtWordEditor
