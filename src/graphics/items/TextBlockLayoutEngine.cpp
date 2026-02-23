#include "graphics/items/TextBlockLayoutEngine.h"
#include "graphics/items/TextFragment.h"
#include <QDebug>

namespace QtWordEditor {

TextBlockLayoutEngine::TextBlockLayoutEngine()
    : m_availableWidth(800)
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
    
    // 2. 计算每行的基线
    calculateLineBaselines();
    
    // 3. 应用文本对齐
    applyAlignment();
    
    // 4. 定位所有项
    positionItems();
    
    // 5. 计算整体尺寸
    calculateTotalSize();
    
    qDebug() << "TextBlockLayoutEngine::layout - completed"
             << "lines:" << m_lines.size()
             << "totalWidth:" << m_totalWidth
             << "totalHeight:" << m_totalHeight;
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
    QList<QRectF> result;
    for (const LineInfo &line : m_lines) {
        result.append(line.rect);
    }
    return result;
}

void TextBlockLayoutEngine::assignItemsToLines(const QList<QGraphicsItem*> &items)
{
    qDebug() << "[DEBUG] TextBlockLayoutEngine::assignItemsToLines - m_availableWidth:" << m_availableWidth;
    qreal currentLineWidth = 0;
    qreal currentY = 0;
    // 使用固定的行间距，因为 lineHeight() 返回的是百分比
    qreal lineSpacing = 5;
    
    LineInfo currentLine;
    currentLine.rect = QRectF(0, currentY, 0, 0);
    currentLine.maxBaseline = 0;
    
    for (QGraphicsItem *item : items) {
        qreal itemWidth = getItemWidth(item);
        qDebug() << "[DEBUG] TextBlockLayoutEngine::assignItemsToLines - itemWidth:" << itemWidth << "currentLineWidth:" << currentLineWidth;
        
        // 检查是否需要换行
        if (currentLineWidth + itemWidth > m_availableWidth && currentLineWidth > 0) {
            qDebug() << "[DEBUG] TextBlockLayoutEngine::assignItemsToLines - wrapping!";
            // 添加当前行到列表
            m_lines.append(currentLine);
            
            // 开始新行
            currentY += currentLine.rect.height() + lineSpacing;
            currentLine = LineInfo();
            currentLine.rect = QRectF(0, currentY, 0, 0);
            currentLineWidth = 0;
        }
        
        // 添加项到当前行
        currentLine.items.append(item);
        currentLineWidth += itemWidth;
        
        // 更新当前行宽度
        if (currentLineWidth > currentLine.rect.width()) {
            currentLine.rect.setWidth(currentLineWidth);
        }
        
        // 更新当前行高度（取最大值）
        qreal itemHeight = getItemHeight(item);
        if (itemHeight > currentLine.rect.height()) {
            currentLine.rect.setHeight(itemHeight);
        }
    }
    
    // 添加最后一行
    if (!currentLine.items.isEmpty()) {
        m_lines.append(currentLine);
    }
    qDebug() << "[DEBUG] TextBlockLayoutEngine::assignItemsToLines - lines:" << m_lines.size();
}

void TextBlockLayoutEngine::calculateLineBaselines()
{
    for (LineInfo &line : m_lines) {
        qreal maxBaseline = 0;
        
        // 计算该行的最大基线
        for (QGraphicsItem *item : line.items) {
            qreal baseline = getItemBaseline(item);
            if (baseline > maxBaseline) {
                maxBaseline = baseline;
            }
        }
        
        line.maxBaseline = maxBaseline;
    }
}

void TextBlockLayoutEngine::applyAlignment()
{
    ParagraphAlignment alignment = m_paragraphStyle.alignment();
    
    for (LineInfo &line : m_lines) {
        qreal availableSpace = m_availableWidth - line.rect.width();
        
        if (availableSpace > 0) {
            qreal offsetX = 0;
            
            switch (alignment) {
                case ParagraphAlignment::AlignLeft:
                    offsetX = 0;
                    break;
                case ParagraphAlignment::AlignCenter:
                    offsetX = availableSpace / 2;
                    break;
                case ParagraphAlignment::AlignRight:
                    offsetX = availableSpace;
                    break;
                case ParagraphAlignment::AlignJustify:
                case ParagraphAlignment::AlignDistributed:
                    // 暂不实现两端对齐
                    offsetX = 0;
                    break;
            }
            
            // 调整行的位置
            line.rect.translate(offsetX, 0);
        }
    }
}

void TextBlockLayoutEngine::positionItems()
{
    for (const LineInfo &line : m_lines) {
        qreal currentX = line.rect.left();
        
        for (QGraphicsItem *item : line.items) {
            qreal itemWidth = getItemWidth(item);
            qreal itemHeight = getItemHeight(item);
            qreal itemBaseline = getItemBaseline(item);
            
            // 计算垂直位置（基线对齐）
            // 行的maxBaseline是从行顶部到基线的距离
            // itemBaseline是从项顶部到基线的距离
            // 所以项的y位置 = 行顶部y + (maxBaseline - itemBaseline)
            qreal y = line.rect.top() + (line.maxBaseline - itemBaseline);
            
            // 保存位置
            m_itemPositions[item] = QPointF(currentX, y);
            
            // 移动到下一个项的位置
            currentX += itemWidth;
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
    
    // 计算总宽度（取最宽的行）
    m_totalWidth = 0;
    for (const LineInfo &line : m_lines) {
        if (line.rect.width() > m_totalWidth) {
            m_totalWidth = line.rect.width();
        }
    }
    
    // 计算总高度（最后一行的底部）
    const LineInfo &lastLine = m_lines.last();
    m_totalHeight = lastLine.rect.bottom();
}

qreal TextBlockLayoutEngine::getItemWidth(QGraphicsItem *item) const
{
    // 尝试转换为TextFragment
    TextFragment *textFragment = dynamic_cast<TextFragment*>(item);
    if (textFragment) {
        return textFragment->width();
    }
    
    // 其他类型的项，使用boundingRect的宽度
    return item->boundingRect().width();
}

qreal TextBlockLayoutEngine::getItemHeight(QGraphicsItem *item) const
{
    // 尝试转换为TextFragment
    TextFragment *textFragment = dynamic_cast<TextFragment*>(item);
    if (textFragment) {
        return textFragment->height();
    }
    
    // 其他类型的项，使用boundingRect的高度
    return item->boundingRect().height();
}

qreal TextBlockLayoutEngine::getItemBaseline(QGraphicsItem *item) const
{
    // 尝试转换为TextFragment
    TextFragment *textFragment = dynamic_cast<TextFragment*>(item);
    if (textFragment) {
        return textFragment->baseline();
    }
    
    // 其他类型的项，默认基线在底部
    return item->boundingRect().height();
}

} // namespace QtWordEditor
