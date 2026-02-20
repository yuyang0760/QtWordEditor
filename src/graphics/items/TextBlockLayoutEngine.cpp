#include "graphics/items/TextBlockLayoutEngine.h"
#include <QTextLayout>
#include <QTextLine>
#include <limits>
#include "core/Global.h"
#include "core/utils/Constants.h"

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
    clear();
    
    if (spans.isEmpty()) {
        return;
    }
    
    // 第一步：为每个 Span 计算它的行，然后将所有行片段收集起来
    QList<LayoutItem> allItems;
    int currentGlobalOffset = 0;
    
    for (int spanIndex = 0; spanIndex < spans.size(); ++spanIndex) {
        const Span &span = spans[spanIndex];
        QString spanText = span.text();
        
        if (spanText.isEmpty()) {
            continue;
        }
        
        // 为这个 Span 创建 QTextLayout
        QFont font = createFontFromStyle(span.style());
        QTextLayout textLayout(spanText, font);
        textLayout.beginLayout();
        
        qreal currentY = 0;
        
        while (true) {
            QTextLine line = textLayout.createLine();
            if (!line.isValid()) {
                break;
            }
            
            line.setLineWidth(m_availableWidth);
            line.setPosition(QPointF(0, currentY));
            
            // 获取这一行的文本范围
            int lineStart = line.textStart();
            int lineEnd = lineStart + line.textLength();
            QString lineText = spanText.mid(lineStart, lineEnd - lineStart);
            
            // 创建 LayoutItem
            LayoutItem item;
            item.spanIndex = spanIndex;
            item.text = lineText;
            item.fullSpanText = spanText;  // 保存整个 Span 的完整文本
            item.style = span.style();
            item.font = font;
            item.width = line.width();
            item.height = line.height();
            item.ascent = line.ascent();
            item.descent = line.descent();
            item.startOffsetInSpan = lineStart;
            item.endOffsetInSpan = lineEnd;
            item.globalStartOffset = currentGlobalOffset + lineStart;
            item.globalEndOffset = currentGlobalOffset + lineEnd;
            
            allItems << item;
            
            currentY += line.height();
        }
        
        textLayout.endLayout();
        
        currentGlobalOffset += spanText.length();
    }
    
    // 第二步：将这些片段分配到行（处理多个 Span 在同一行的情况）
    // 这里我们简化处理：按顺序分配，每一行尽可能装下更多的片段
    qreal currentX = 0;
    qreal currentY = 0;
    qreal currentLineMaxHeight = 0;
    qreal currentLineMaxAscent = 0;
    QList<int> currentLineItemIndices;
    
    for (int itemIndex = 0; itemIndex < allItems.size(); ++itemIndex) {
        LayoutItem &item = allItems[itemIndex];
        
        // 检查这个片段是否能放到当前行
        if (currentX + item.width > m_availableWidth && !currentLineItemIndices.isEmpty()) {
            // 不能放到当前行，先结束当前行
            finishLine(currentLineItemIndices, allItems, currentX, currentY, currentLineMaxHeight, currentLineMaxAscent);
            
            // 开始新行
            currentX = 0;
            currentY += currentLineMaxHeight;
            currentLineMaxHeight = 0;
            currentLineMaxAscent = 0;
            currentLineItemIndices.clear();
        }
        
        // 放到当前行
        currentLineItemIndices << itemIndex;
        if (item.height > currentLineMaxHeight) {
            currentLineMaxHeight = item.height;
        }
        if (item.ascent > currentLineMaxAscent) {
            currentLineMaxAscent = item.ascent;
        }
        currentX += item.width;
    }
    
    // 结束最后一行
    if (!currentLineItemIndices.isEmpty()) {
        finishLine(currentLineItemIndices, allItems, currentX, currentY, currentLineMaxHeight, currentLineMaxAscent);
    }
    
    // 保存所有 LayoutItem
    m_layoutItems = allItems;
    
    // 计算总尺寸
    m_totalWidth = m_availableWidth;
    if (!m_lines.isEmpty()) {
        m_totalHeight = m_lines.last().rect.bottom();
    }
}

void TextBlockLayoutEngine::finishLine(QList<int> &itemIndices, QList<LayoutItem> &allItems, 
                                        qreal lineWidth, qreal lineY, qreal lineHeight, qreal lineMaxAscent)
{
    // 创建 LineInfo
    LineInfo lineInfo;
    lineInfo.rect = QRectF(0, lineY, m_availableWidth, lineHeight);
    lineInfo.maxBaseline = lineMaxAscent;
    lineInfo.itemIndices = itemIndices;
    
    // 定位这一行中的每个 item
    qreal currentX = 0;
    for (int itemIndex : itemIndices) {
        LayoutItem &item = allItems[itemIndex];
        // 计算基线对齐的 Y 位置
        qreal y = lineY + (lineMaxAscent - item.ascent);
        item.position = QPointF(currentX, y);
        currentX += item.width;
    }
    
    m_lines << lineInfo;
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
    Q_UNUSED(spans);
    
    CursorHitResult result;
    result.globalOffset = 0;
    result.lineIndex = -1;
    result.itemIndex = 0;
    result.offsetInItem = 0;
    
    if (m_lines.isEmpty() || m_layoutItems.isEmpty()) {
        return result;
    }
    
    // 第一步：找到所在的行
    int foundLineIndex = -1;
    for (int i = 0; i < m_lines.size(); ++i) {
        const LineInfo &line = m_lines[i];
        if (localPos.y() >= line.rect.top() && localPos.y() < line.rect.bottom()) {
            foundLineIndex = i;
            break;
        }
    }
    
    // 如果没找到行，找最近的
    if (foundLineIndex == -1) {
        if (localPos.y() < m_lines.first().rect.top()) {
            foundLineIndex = 0;
        } else {
            foundLineIndex = m_lines.size() - 1;
        }
    }
    
    result.lineIndex = foundLineIndex;
    const LineInfo &line = m_lines[foundLineIndex];
    
    // 第二步：在这一行中找到所在的 item
    int foundItemIndex = -1;
    for (int itemIndex : line.itemIndices) {
        const LayoutItem &item = m_layoutItems[itemIndex];
        if (localPos.x() >= item.position.x() && localPos.x() < item.position.x() + item.width) {
            foundItemIndex = itemIndex;
            break;
        }
    }
    
    // 如果没找到 item，找最近的
    if (foundItemIndex == -1) {
        if (localPos.x() < m_layoutItems[line.itemIndices.first()].position.x()) {
            foundItemIndex = line.itemIndices.first();
        } else {
            foundItemIndex = line.itemIndices.last();
        }
    }
    
    result.itemIndex = foundItemIndex;
    const LayoutItem &item = m_layoutItems[foundItemIndex];
    
    // 第三步：在 item 中计算偏移
    QTextLayout textLayout(item.text, item.font);
    textLayout.beginLayout();
    QTextLine textLine = textLayout.createLine();
    textLine.setLineWidth(item.width);
    textLayout.endLayout();
    
    qreal localX = localPos.x() - item.position.x();
    int offsetInItem = textLine.xToCursor(localX);  // 应该用 xToCursor，不是 cursorToX！
    offsetInItem = qBound(0, offsetInItem, item.text.length());
    
    result.offsetInItem = offsetInItem;
    result.globalOffset = item.globalStartOffset + offsetInItem;
    
    return result;
}

TextBlockLayoutEngine::CursorVisualResult TextBlockLayoutEngine::cursorPositionAt(int globalOffset, const QList<Span> &spans) const
{
    Q_UNUSED(spans);
    
    CursorVisualResult result;
    result.position = QPointF(0, 0);
    result.height = 20;
    
    if (m_layoutItems.isEmpty()) {
        return result;
    }
    
    // 找到包含这个偏移的 item
    int foundItemIndex = -1;
    for (int i = 0; i < m_layoutItems.size(); ++i) {
        const LayoutItem &item = m_layoutItems[i];
        if (globalOffset >= item.globalStartOffset && globalOffset <= item.globalEndOffset) {
            foundItemIndex = i;
            break;
        }
    }
    
    // 如果没找到，找最近的
    if (foundItemIndex == -1) {
        if (globalOffset < m_layoutItems.first().globalStartOffset) {
            foundItemIndex = 0;
        } else {
            foundItemIndex = m_layoutItems.size() - 1;
        }
    }
    
    const LayoutItem &item = m_layoutItems[foundItemIndex];
    result.height = item.height;
    
    // 计算在 item 中的偏移
    int offsetInItem = globalOffset - item.globalStartOffset;
    offsetInItem = qBound(0, offsetInItem, item.text.length());
    
    // 使用 QTextLayout 计算 X 位置
    QTextLayout textLayout(item.text, item.font);
    textLayout.beginLayout();
    QTextLine textLine = textLayout.createLine();
    textLine.setLineWidth(item.width);
    textLayout.endLayout();
    
    qreal x = textLine.cursorToX(offsetInItem);
    result.position = QPointF(item.position.x() + x, item.position.y());
    
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
    } else {
        font.setPointSize(Constants::DefaultFontSize);
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

void TextBlockLayoutEngine::clear()
{
    m_layoutItems.clear();
    m_lines.clear();
    m_totalWidth = 0;
    m_totalHeight = 0;
}

} // namespace QtWordEditor
