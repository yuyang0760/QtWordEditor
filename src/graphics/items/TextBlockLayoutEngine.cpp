#include "graphics/items/TextBlockLayoutEngine.h"
#include "core/document/TextSpan.h"
#include <QTextLayout>
#include <QTextLine>
#include <QFontMetricsF>
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

void TextBlockLayoutEngine::layout(const QList<InlineSpan*> &spans)
{
    clear();
    
    if (spans.isEmpty()) {
        return;
    }
    
    // ========== 调试信息：打印传入的 spans ==========
    qDebug() << "TextBlockLayoutEngine::layout() - 开始布局，spans 数量:" << spans.size();
    for (int i = 0; i < spans.size(); ++i) {
        if (spans[i]->type() == InlineSpan::Text) {
            const TextSpan *textSpan = static_cast<const TextSpan*>(spans[i]);
            qDebug() << "  span" << i << ":" 
                     << "text=[" << textSpan->text() << "]" 
                     << "bold=" << textSpan->style().bold() 
                     << "length=" << textSpan->text().length();
        }
    }
    qDebug() << "  availableWidth:" << m_availableWidth;
    // =========================================
    
    // 重新设计的布局逻辑：
    // 1. 先为每个 InlineSpan 创建完整的 item（不分割）
    // 2. 然后逐行放置这些 items，在需要换行时再分割 item
    
    QList<LayoutItem> allItems;
    int currentGlobalOffset = 0;
    
    // 第一步：为每个 InlineSpan 创建完整的 item（不分割）
    for (int spanIndex = 0; spanIndex < spans.size(); ++spanIndex) {
        InlineSpan *span = spans[spanIndex];
        
        if (span->type() != InlineSpan::Text) {
            continue; // 暂时只处理 TextSpan
        }
        
        TextSpan *textSpan = static_cast<TextSpan*>(span);
        QString spanText = textSpan->text();
        
        if (spanText.isEmpty()) {
            continue;
        }
        
        QFont font = createFontFromStyle(textSpan->style());
        QFontMetricsF fontMetrics(font);
        QTextLayout textLayout(spanText, font);
        textLayout.beginLayout();
        QTextLine textLine = textLayout.createLine();
        textLine.setLineWidth(std::numeric_limits<qreal>::max()); // 不限制宽度，获取完整高度
        textLayout.endLayout();
        
        // 创建完整的 LayoutItem
        LayoutItem item;
        item.spanIndex = spanIndex;
        item.inlineSpan = span;
        item.text = spanText;
        item.fullSpanText = spanText;
        item.style = textSpan->style();
        item.font = font;
        // 使用 QTextLayout 计算宽度，比 QFontMetricsF 更准确，特别是对于斜体字体
        item.width = textLine.naturalTextWidth();
        item.height = textLine.height();
        item.ascent = textLine.ascent();
        item.descent = textLine.descent();
        item.startOffsetInSpan = 0;
        item.endOffsetInSpan = spanText.length();
        item.globalStartOffset = currentGlobalOffset;
        item.globalEndOffset = currentGlobalOffset + spanText.length();
        
        allItems << item;
        
        currentGlobalOffset += spanText.length();
    }
    
    // 第二步：逐行放置 items，在需要换行时分割 item
    qreal currentLineX = 0;
    qreal currentLineY = 0;
    QList<LayoutItem> finalItems;
    int itemIndex = 0;
    
    while (itemIndex < allItems.size()) {
        LayoutItem &currentItem = allItems[itemIndex];
        QFont font = currentItem.font;
        QFontMetricsF fontMetrics(font);
        
        // 计算这个 item 还能在当前行放多少字符
        qreal remainingWidth = m_availableWidth - currentLineX;
        
        if (remainingWidth <= 0) {
            // 当前行已满，换行
            finishLineFromItems(finalItems, currentLineY);
            
            currentLineX = 0;
            if (!finalItems.isEmpty()) {
                currentLineY += finalItems.last().height;
            }
            finalItems.clear();
            continue;
        }
        
        // 找到最多能放的字符数
        int charsToFit = 0;
        QString textToFit;
        
        if (currentLineX == 0) {
            // 新行的第一个 item，尝试找到能放下的最大字符数
            for (int i = 1; i <= currentItem.text.length(); ++i) {
                QString testText = currentItem.text.left(i);
                // 使用 QTextLayout 计算测试文本宽度
                QTextLayout testLayout(testText, currentItem.font);
                testLayout.beginLayout();
                QTextLine testLine = testLayout.createLine();
                testLine.setLineWidth(std::numeric_limits<qreal>::max());
                testLayout.endLayout();
                qreal testWidth = testLine.naturalTextWidth();
                
                if (testWidth <= m_availableWidth) {
                    charsToFit = i;
                    textToFit = testText;
                } else {
                    break;
                }
            }
        } else {
            // 不是新行的第一个 item，尝试找到能放下的最大字符数
            for (int i = 1; i <= currentItem.text.length(); ++i) {
                QString testText = currentItem.text.left(i);
                // 使用 QTextLayout 计算测试文本宽度
                QTextLayout testLayout(testText, currentItem.font);
                testLayout.beginLayout();
                QTextLine testLine = testLayout.createLine();
                testLine.setLineWidth(std::numeric_limits<qreal>::max());
                testLayout.endLayout();
                qreal testWidth = testLine.naturalTextWidth();
                
                if (testWidth <= remainingWidth) {
                    charsToFit = i;
                    textToFit = testText;
                } else {
                    break;
                }
            }
        }
        
        if (charsToFit == 0 && currentLineX == 0) {
            // 即使新行也放不下一个字符，强制放一个字符
            charsToFit = 1;
            textToFit = currentItem.text.left(1);
        }
        
        if (charsToFit > 0) {
            // 创建新的 item 来放这部分文本
            LayoutItem newItem;
            newItem.spanIndex = currentItem.spanIndex;
            newItem.inlineSpan = currentItem.inlineSpan;
            newItem.text = textToFit;
            newItem.fullSpanText = currentItem.fullSpanText;
            newItem.style = currentItem.style;
            newItem.font = currentItem.font;
            // 使用 QTextLayout 计算宽度
            QTextLayout tempLayout(textToFit, currentItem.font);
            tempLayout.beginLayout();
            QTextLine tempLine = tempLayout.createLine();
            tempLine.setLineWidth(std::numeric_limits<qreal>::max());
            tempLayout.endLayout();
            newItem.width = tempLine.naturalTextWidth();
            newItem.height = currentItem.height;
            newItem.ascent = currentItem.ascent;
            newItem.descent = currentItem.descent;
            newItem.startOffsetInSpan = currentItem.startOffsetInSpan;
            newItem.endOffsetInSpan = currentItem.startOffsetInSpan + charsToFit;
            newItem.globalStartOffset = currentItem.globalStartOffset;
            newItem.globalEndOffset = currentItem.globalStartOffset + charsToFit;
            
            finalItems << newItem;
            currentLineX += newItem.width;
            
            // 检查是否用完了这个 item 的所有字符
            if (charsToFit < currentItem.text.length()) {
                // 还有剩余字符，更新 currentItem 为剩余部分
                QString remainingText = currentItem.text.mid(charsToFit);
                currentItem.text = remainingText;
                // 使用 QTextLayout 计算剩余部分的宽度
                QTextLayout remainingLayout(remainingText, currentItem.font);
                remainingLayout.beginLayout();
                QTextLine remainingLine = remainingLayout.createLine();
                remainingLine.setLineWidth(std::numeric_limits<qreal>::max());
                remainingLayout.endLayout();
                currentItem.width = remainingLine.naturalTextWidth();
                currentItem.startOffsetInSpan += charsToFit;
                currentItem.globalStartOffset += charsToFit;
                
                // 换行
                finishLineFromItems(finalItems, currentLineY);
                
                currentLineX = 0;
                if (!finalItems.isEmpty()) {
                    currentLineY += finalItems.last().height;
                }
                finalItems.clear();
            } else {
                // 用完了这个 item，继续下一个
                itemIndex++;
            }
        } else {
            // 放不下了，换行
            finishLineFromItems(finalItems, currentLineY);
            
            currentLineX = 0;
            if (!finalItems.isEmpty()) {
                currentLineY += finalItems.last().height;
            }
            finalItems.clear();
        }
    }
    
    // 结束最后一行
    if (!finalItems.isEmpty()) {
        finishLineFromItems(finalItems, currentLineY);
    }
    
    // ========== 调试信息：打印最终的行和 items ==========
    qDebug() << "TextBlockLayoutEngine::layout() - 最终行数量:" << m_lines.size();
    for (int i = 0; i < m_lines.size(); ++i) {
        qDebug() << "  line" << i << ":" 
                 << "rect=" << m_lines[i].rect 
                 << "itemIndices=" << m_lines[i].itemIndices;
    }
    qDebug() << "TextBlockLayoutEngine::layout() - 最终 items 数量:" << m_layoutItems.size();
    for (int i = 0; i < m_layoutItems.size(); ++i) {
        qDebug() << "  m_layoutItems[" << i << "]:" 
                 << "text=[" << m_layoutItems[i].text << "]" 
                 << "spanIndex=" << m_layoutItems[i].spanIndex 
                 << "position=" << m_layoutItems[i].position 
                 << "width=" << m_layoutItems[i].width;
    }
    // =========================================
    
    // 计算总尺寸
    m_totalWidth = m_availableWidth;
    if (!m_lines.isEmpty()) {
        m_totalHeight = m_lines.last().rect.bottom();
    }
}

void TextBlockLayoutEngine::finishLine(QList<int> &itemIndices, QList<LayoutItem> &allItems, 
                                        qreal lineWidth, qreal lineY, qreal lineHeight, qreal lineMaxAscent)
{
    LineInfo lineInfo;
    lineInfo.rect = QRectF(0, lineY, lineWidth, lineHeight);
    lineInfo.maxBaseline = lineMaxAscent;
    lineInfo.itemIndices = itemIndices;
    
    // 为这一行中的每个 item 设置位置（基线对齐）
    qreal currentX = 0;
    for (int itemIndex : itemIndices) {
        LayoutItem &item = allItems[itemIndex];
        qreal itemY = lineY + (lineMaxAscent - item.ascent);
        item.position = QPointF(currentX, itemY);
        currentX += item.width;
    }
    
    m_lines.append(lineInfo);
}

void TextBlockLayoutEngine::finishLineFromItems(QList<LayoutItem> &items, qreal lineY)
{
    if (items.isEmpty()) {
        return;
    }
    
    // 计算这一行的最大高度和最大 ascent
    qreal lineMaxHeight = 0;
    qreal lineMaxAscent = 0;
    qreal lineWidth = 0;
    
    for (const LayoutItem &item : items) {
        if (item.height > lineMaxHeight) {
            lineMaxHeight = item.height;
        }
        if (item.ascent > lineMaxAscent) {
            lineMaxAscent = item.ascent;
        }
        lineWidth += item.width;
    }
    
    // 创建 LineInfo
    LineInfo lineInfo;
    lineInfo.rect = QRectF(0, lineY, lineWidth, lineMaxHeight);
    lineInfo.maxBaseline = lineMaxAscent;
    
    // 为这一行中的每个 item 设置位置（基线对齐），并添加到 m_layoutItems
    qreal currentX = 0;
    for (LayoutItem &item : items) {
        qreal itemY = lineY + (lineMaxAscent - item.ascent);
        item.position = QPointF(currentX, itemY);
        
        // 添加到 m_layoutItems
        lineInfo.itemIndices.append(m_layoutItems.size());
        m_layoutItems.append(item);
        
        currentX += item.width;
    }
    
    m_lines.append(lineInfo);
}

QFont TextBlockLayoutEngine::createFontFromStyle(const CharacterStyle &style) const
{
    QFont font;
    
    // 应用字体族
    if (style.isPropertySet(CharacterStyleProperty::FontFamily)) {
        font.setFamily(style.fontFamily());
    }
    
    // 应用字号
    if (style.isPropertySet(CharacterStyleProperty::FontSize)) {
        font.setPointSizeF(style.fontSize());
    } else {
        font.setPointSize(Constants::DefaultFontSize);
    }
    
    // 应用加粗
    font.setBold(style.bold());
    
    // 应用斜体
    font.setItalic(style.italic());
    
    // 应用下划线
    font.setUnderline(style.underline());
    
    return font;
}

void TextBlockLayoutEngine::clear()
{
    m_layoutItems.clear();
    m_lines.clear();
    m_totalWidth = 0;
    m_totalHeight = 0;
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

QList<QRectF> TextBlockLayoutEngine::selectionRects(int startOffset, int endOffset, const QList<InlineSpan*> &spans) const
{
    Q_UNUSED(spans);
    QList<QRectF> rects;
    
    // 归一化偏移
    int normalizedStart = qMin(startOffset, endOffset);
    int normalizedEnd = qMax(startOffset, endOffset);
    
    if (normalizedStart >= normalizedEnd) {
        return rects;
    }
    
    // 按行处理，确保选择背景色连续
    for (int lineIdx = 0; lineIdx < m_lines.size(); ++lineIdx) {
        const LineInfo &lineInfo = m_lines[lineIdx];
        
        // 找到这一行的最小和最大全局偏移
        int lineMinOffset = INT_MAX;
        int lineMaxOffset = -1;
        
        // 遍历这一行的所有 item
        for (int itemIndex : lineInfo.itemIndices) {
            const LayoutItem &item = m_layoutItems[itemIndex];
            lineMinOffset = qMin(lineMinOffset, item.globalStartOffset);
            lineMaxOffset = qMax(lineMaxOffset, item.globalEndOffset);
        }
        
        // 检查这一行是否与选择范围重叠
        if (lineMaxOffset <= normalizedStart || lineMinOffset >= normalizedEnd) {
            continue;
        }
        
        // 计算这一行在选择范围内的起始和结束位置
        int selStartInLine = qMax(normalizedStart, lineMinOffset);
        int selEndInLine = qMin(normalizedEnd, lineMaxOffset);
        
        // 找到选择起始位置对应的 item 和 X 坐标
        qreal startX = 0;
        qreal endX = 0;
        bool foundStart = false;
        bool foundEnd = false;
        
        // 先找起始位置
        for (int itemIndex : lineInfo.itemIndices) {
            const LayoutItem &item = m_layoutItems[itemIndex];
            
            if (!foundStart && selStartInLine >= item.globalStartOffset && selStartInLine <= item.globalEndOffset) {
                // 找到起始位置所在的 item
                int offsetInItem = selStartInLine - item.globalStartOffset;
                QTextLayout textLayout(item.text, item.font);
                textLayout.beginLayout();
                QTextLine textLine = textLayout.createLine();
                textLine.setLineWidth(item.width);
                textLayout.endLayout();
                startX = item.position.x() + textLine.cursorToX(offsetInItem);
                foundStart = true;
            }
            
            if (!foundEnd && selEndInLine >= item.globalStartOffset && selEndInLine <= item.globalEndOffset) {
                // 找到结束位置所在的 item
                int offsetInItem = selEndInLine - item.globalStartOffset;
                QTextLayout textLayout(item.text, item.font);
                textLayout.beginLayout();
                QTextLine textLine = textLayout.createLine();
                textLine.setLineWidth(item.width);
                textLayout.endLayout();
                endX = item.position.x() + textLine.cursorToX(offsetInItem);
                foundEnd = true;
            }
            
            if (foundStart && foundEnd) {
                break;
            }
        }
        
        // 如果起始或结束位置没有找到（比如在整行都被选中的情况下），使用行的边界
        if (!foundStart) {
            // 整行从开头选中，使用第一个 item 的 x 位置
            if (!lineInfo.itemIndices.isEmpty()) {
                const LayoutItem &firstItem = m_layoutItems[lineInfo.itemIndices.first()];
                startX = firstItem.position.x();
            }
        }
        
        if (!foundEnd) {
            // 整行选中到末尾，使用最后一个 item 的 x + width 位置
            if (!lineInfo.itemIndices.isEmpty()) {
                const LayoutItem &lastItem = m_layoutItems[lineInfo.itemIndices.last()];
                endX = lastItem.position.x() + lastItem.width;
            }
        }
        
        // 创建这一行的连续选择矩形
        QRectF lineSelectionRect(
            startX,
            lineInfo.rect.top(),
            endX - startX,
            lineInfo.rect.height()
        );
        
        rects << lineSelectionRect;
    }
    
    return rects;
}

TextBlockLayoutEngine::CursorHitResult TextBlockLayoutEngine::hitTest(const QPointF &localPos, const QList<InlineSpan*> &spans) const
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
    int offsetInItem = textLine.xToCursor(localX);
    offsetInItem = qBound(0, offsetInItem, item.text.length());
    
    result.offsetInItem = offsetInItem;
    result.globalOffset = item.globalStartOffset + offsetInItem;
    
    return result;
}

TextBlockLayoutEngine::CursorVisualResult TextBlockLayoutEngine::cursorPositionAt(int globalOffset, const QList<InlineSpan*> &spans) const
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
    
    // 在 item 中计算位置
    QTextLayout textLayout(item.text, item.font);
    textLayout.beginLayout();
    QTextLine textLine = textLayout.createLine();
    textLine.setLineWidth(item.width);
    textLayout.endLayout();
    
    int offsetInItem = globalOffset - item.globalStartOffset;
    offsetInItem = qBound(0, offsetInItem, item.text.length());
    
    qreal x = item.position.x() + textLine.cursorToX(offsetInItem);
    qreal baselineY = item.position.y() + item.ascent;
    
    result.position = QPointF(x, item.position.y());
    result.height = item.height;
    
    return result;
}

} // namespace QtWordEditor
