#include "graphics/items/TextBlockLayoutEngine.h"
#include "graphics/items/TextFragment.h"
#include <QDebug>

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

void TextBlockLayoutEngine::layout(const QList<QGraphicsItem*> &items)
{
    // 清除之前的布局结果
    clear();
    
    // 1. 将项分配到行
    assignItemsToLines(items);
    
    // 2. 计算每一行的基线
    calculateLineBaselines();
    
    // 3. 应用文本对齐
    applyAlignment();
    
    // 4. 定位所有项
    positionItems();
    
    // 5. 计算整体尺寸
    calculateTotalSize();
}

void TextBlockLayoutEngine::clear()
{
    m_lines.clear();
    m_itemPositions.clear();
    m_totalWidth = 0;
    m_totalHeight = 0;
}

qreal TextBlockLayoutEngine::totalHeight() const
{
    return m_totalHeight;
}

qreal TextBlockLayoutEngine::totalWidth() const
{
    return m_totalWidth;
}

QPointF TextBlockLayoutEngine::positionForItem(QGraphicsItem *item) const
{
    return m_itemPositions.value(item, QPointF(0, 0));
}

QList<QRectF> TextBlockLayoutEngine::lines() const
{
    QList<QRectF> rects;
    for (const LineInfo &line : m_lines) {
        rects << line.rect;
    }
    return rects;
}

void TextBlockLayoutEngine::assignItemsToLines(const QList<QGraphicsItem*> &items)
{
    if (items.isEmpty()) {
        return;
    }
    
    LineInfo currentLine;
    currentLine.rect = QRectF(0, 0, 0, 0);
    currentLine.maxBaseline = 0;
    
    qreal currentY = 0;
    
    for (QGraphicsItem *item : items) {
        qreal itemW = itemWidth(item);
        qreal itemH = itemHeight(item);
        qreal itemBase = itemBaseline(item);
        
        // 计算如果加入当前行的总宽度
        qreal newWidth = currentLine.rect.width() + itemW;
        
        // 检查是否需要换行
        bool needWrap = false;
        if (m_wrapMode != WrapMode::NoWrap && m_availableWidth > 0) {
            if (newWidth > m_availableWidth && !currentLine.items.isEmpty()) {
                needWrap = true;
            }
        }
        
        if (needWrap) {
            // 完成当前行
            currentLine.rect = QRectF(0, currentY, currentLine.rect.width(), currentLine.rect.height());
            m_lines << currentLine;
            currentY += currentLine.rect.height();
            
            // 开始新行
            currentLine = LineInfo();
            currentLine.rect = QRectF(0, 0, itemW, itemH);
            currentLine.maxBaseline = itemBase;
            currentLine.items << item;
        } else {
            // 加入当前行
            currentLine.items << item;
            currentLine.rect.setWidth(newWidth);
            
            // 更新行高度和最大基线
            if (itemH > currentLine.rect.height()) {
                currentLine.rect.setHeight(itemH);
            }
            if (itemBase > currentLine.maxBaseline) {
                currentLine.maxBaseline = itemBase;
            }
        }
    }
    
    // 完成最后一行
    if (!currentLine.items.isEmpty()) {
        currentLine.rect = QRectF(0, currentY, currentLine.rect.width(), currentLine.rect.height());
        m_lines << currentLine;
    }
}

void TextBlockLayoutEngine::calculateLineBaselines()
{
    // 基线已经在 assignItemsToLines 中计算了 maxBaseline
    // 这里我们只需要确保行高足够容纳所有项
    for (LineInfo &line : m_lines) {
        qreal maxHeight = 0;
        for (QGraphicsItem *item : line.items) {
            qreal itemH = itemHeight(item);
            if (itemH > maxHeight) {
                maxHeight = itemH;
            }
        }
        line.rect.setHeight(maxHeight);
    }
}

void TextBlockLayoutEngine::applyAlignment()
{
    ParagraphAlignment alignment = m_paragraphStyle.alignment();
    
    for (LineInfo &line : m_lines) {
        qreal lineWidth = line.rect.width();
        qreal offsetX = 0;
        
        switch (alignment) {
            case ParagraphAlignment::AlignLeft:
                offsetX = 0;
                break;
            case ParagraphAlignment::AlignCenter:
                offsetX = (m_availableWidth - lineWidth) / 2;
                break;
            case ParagraphAlignment::AlignRight:
                offsetX = m_availableWidth - lineWidth;
                break;
            case ParagraphAlignment::AlignJustify:
                // 两端对齐：最后一行左对齐
                if (&line != &m_lines.last()) {
                    // 这里可以实现更复杂的两端对齐
                    offsetX = 0;
                } else {
                    offsetX = 0;
                }
                break;
            case ParagraphAlignment::AlignDistributed:
                offsetX = 0;
                break;
        }
        
        // 偏移行的X坐标
        line.rect.translate(offsetX, 0);
    }
}

void TextBlockLayoutEngine::positionItems()
{
    for (LineInfo &line : m_lines) {
        qreal currentX = line.rect.left();
        
        for (QGraphicsItem *item : line.items) {
            qreal itemW = itemWidth(item);
            qreal itemBase = itemBaseline(item);
            
            // 计算Y位置：基线对齐
            qreal itemY = line.rect.top() + (line.maxBaseline - itemBase);
            
            // 保存位置
            m_itemPositions[item] = QPointF(currentX, itemY);
            
            // 更新X坐标
            currentX += itemW;
        }
    }
}

void TextBlockLayoutEngine::calculateTotalSize()
{
    if (m_lines.isEmpty()) {
        m_totalWidth = 0;
        m_totalHeight = 0;
        return;
    }
    
    m_totalWidth = m_availableWidth;
    m_totalHeight = m_lines.last().rect.bottom();
}

qreal TextBlockLayoutEngine::itemWidth(QGraphicsItem *item) const
{
    TextFragment *fragment = qgraphicsitem_cast<TextFragment*>(item);
    if (fragment) {
        return fragment->width();
    }
    
    // 其他类型的项，使用 boundingRect
    return item->boundingRect().width();
}

qreal TextBlockLayoutEngine::itemHeight(QGraphicsItem *item) const
{
    TextFragment *fragment = qgraphicsitem_cast<TextFragment*>(item);
    if (fragment) {
        return fragment->height();
    }
    
    // 其他类型的项，使用 boundingRect
    return item->boundingRect().height();
}

qreal TextBlockLayoutEngine::itemBaseline(QGraphicsItem *item) const
{
    TextFragment *fragment = qgraphicsitem_cast<TextFragment*>(item);
    if (fragment) {
        return fragment->baseline();
    }
    
    // 其他类型的项，默认基线在底部
    return item->boundingRect().height();
}

} // namespace QtWordEditor
