#include "graphics/items/TextBlockLayoutEngine.h"
#include <QDebug>
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
    
    // 1. 为每个 Span 创建 LayoutItem，计算内部多行
    m_layoutItems.clear();
    for (int i = 0; i < spans.count(); ++i) {
        const Span &span = spans[i];
        LayoutItem item = calculateLayoutItem(span, m_availableWidth);
        item.spanIndex = i;
        item.position = QPointF(0, 0);  // 先初始化为 0
        m_layoutItems << item;
    }
    
    // 2. 计算每个 LayoutItem 的位置（Y 坐标累加）
    qreal currentY = 0;
    for (int i = 0; i < m_layoutItems.size(); ++i) {
        LayoutItem &item = m_layoutItems[i];
        item.position = QPointF(0, currentY);
        currentY += item.height;
    }
    
    m_totalHeight = currentY;
    m_totalWidth = m_availableWidth;
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
    result.itemIndex = -1;
    result.offsetInItem = 0;
    
    Q_UNUSED(spans);
    
    if (m_layoutItems.isEmpty()) {
        return result;
    }
    
    qreal x = localPos.x();
    qreal y = localPos.y();
    
    // 找到点击的 LayoutItem
    int hitItemIndex = -1;
    for (int i = 0; i < m_layoutItems.size(); ++i) {
        const LayoutItem &item = m_layoutItems[i];
        qreal itemTop = item.position.y();
        qreal itemBottom = itemTop + item.height;
        
        if (y >= itemTop && y <= itemBottom) {
            hitItemIndex = i;
            break;
        }
    }
    
    if (hitItemIndex == -1) {
        // 没找到，用第一个或最后一个
        if (y < m_layoutItems.first().position.y()) {
            hitItemIndex = 0;
        } else {
            hitItemIndex = m_layoutItems.size() - 1;
        }
    }
    
    const LayoutItem &item = m_layoutItems[hitItemIndex];
    
    // 在该 item 内部找到点击的偏移
    qreal localX = x - item.position.x();
    qreal localY = y - item.position.y();
    
    // 使用临时的 QTextLayout 来计算
    int offsetInItem = 0;
    QTextLayout textLayout(item.text, item.font);
    textLayout.beginLayout();
    
    qreal currentY = 0;
    while (true) {
        QTextLine line = textLayout.createLine();
        if (!line.isValid()) break;
        
        line.setLineWidth(m_availableWidth);
        line.setPosition(QPointF(0, currentY));
        
        qreal lineTop = line.y();
        qreal lineBottom = lineTop + line.height();
        
        if (localY >= lineTop && localY <= lineBottom) {
            offsetInItem = line.textStart() + line.xToCursor(localX);
            break;
        }
        
        currentY += line.height();
    }
    
    textLayout.endLayout();
    
    // 计算全局偏移
    int globalOffset = 0;
    for (int i = 0; i < hitItemIndex; ++i) {
        globalOffset += m_layoutItems[i].text.length();
    }
    globalOffset += offsetInItem;
    
    result.globalOffset = globalOffset;
    result.lineIndex = 0;
    result.itemIndex = hitItemIndex;
    result.offsetInItem = offsetInItem;
    
    return result;
}

TextBlockLayoutEngine::CursorVisualResult TextBlockLayoutEngine::cursorPositionAt(int globalOffset, const QList<Span> &spans) const
{
    CursorVisualResult result;
    result.position = QPointF(0, 0);
    result.height = 20;
    
    Q_UNUSED(spans);
    
    if (m_layoutItems.isEmpty()) {
        return result;
    }
    
    // 找到包含这个偏移的 LayoutItem
    int currentOffset = 0;
    int itemIndex = -1;
    int offsetInItem = 0;
    
    for (int i = 0; i < m_layoutItems.size(); ++i) {
        const LayoutItem &item = m_layoutItems[i];
        int itemLength = item.text.length();
        
        if (globalOffset >= currentOffset && globalOffset <= currentOffset + itemLength) {
            itemIndex = i;
            offsetInItem = globalOffset - currentOffset;
            break;
        }
        
        currentOffset += itemLength;
    }
    
    if (itemIndex == -1) {
        // 没找到，用最后一个
        itemIndex = m_layoutItems.size() - 1;
        offsetInItem = m_layoutItems.last().text.length();
    }
    
    const LayoutItem &item = m_layoutItems[itemIndex];
    
    // 在该 item 内部找到光标位置
    offsetInItem = qBound(0, offsetInItem, item.text.length());
    
    // 使用临时的 QTextLayout 来计算
    QPointF localPos(0, 0);
    QTextLayout textLayout(item.text, item.font);
    textLayout.beginLayout();
    
    qreal currentY = 0;
    while (true) {
        QTextLine line = textLayout.createLine();
        if (!line.isValid()) break;
        
        line.setLineWidth(m_availableWidth);
        line.setPosition(QPointF(0, currentY));
        
        int lineStart = line.textStart();
        int lineEnd = lineStart + line.textLength();
        
        if (offsetInItem >= lineStart && offsetInItem <= lineEnd) {
            qreal x = line.cursorToX(offsetInItem - lineStart);
            qreal y = line.y();
            localPos = QPointF(x, y);
            break;
        }
        
        currentY += line.height();
    }
    
    textLayout.endLayout();
    
    result.position = item.position + localPos;
    
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
