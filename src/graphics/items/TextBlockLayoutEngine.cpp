#include "graphics/items/TextBlockLayoutEngine.h"
#include "graphics/items/TextFragment.h"
#include <QDebug>
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

TextBlockLayoutEngine::CursorHitResult TextBlockLayoutEngine::hitTest(const QPointF &localPos, const QList<QGraphicsItem*> &items) const
{
    CursorHitResult result;
    result.fragment = nullptr;
    result.offset = 0;
    result.globalOffset = 0;
    
    if (m_lines.isEmpty() || items.isEmpty()) {
        return result;
    }
    
    // 首先找到鼠标所在的行
    const LineInfo *targetLine = nullptr;
    for (const LineInfo &line : m_lines) {
        if (localPos.y() <= line.rect.bottom()) {
            targetLine = &line;
            break;
        }
    }
    
    // 如果鼠标在所有行下方，使用最后一行
    if (!targetLine) {
        targetLine = &m_lines.last();
    }
    
    // 然后在该行中找到最接近鼠标 X 坐标的 TextFragment
    qreal minDistance = std::numeric_limits<qreal>::max();
    int globalOffsetCounter = 0;
    
    // 先计算目标行之前的所有字符数
    for (const LineInfo &line : m_lines) {
        if (&line == targetLine) {
            break;
        }
        for (QGraphicsItem *item : line.items) {
            TextFragment *fragment = qgraphicsitem_cast<TextFragment*>(item);
            if (fragment) {
                globalOffsetCounter += fragment->text().length();
            }
        }
    }
    
    // 遍历目标行中的项
    for (QGraphicsItem *item : targetLine->items) {
        TextFragment *fragment = qgraphicsitem_cast<TextFragment*>(item);
        if (!fragment) {
            continue;
        }
        
        // 获取 fragment 的位置
        QPointF fragmentPos = positionForItem(item);
        
        // 将鼠标坐标转换为 fragment 的局部坐标
        QPointF fragmentLocalPos = localPos - fragmentPos;
        
        // 使用 fragment 的 hitTest
        int fragmentOffset = fragment->hitTest(fragmentLocalPos);
        
        // 计算该 fragment 中这个位置的全局 X 坐标
        QPointF cursorLocalPos = fragment->cursorPositionAt(fragmentOffset);
        qreal globalX = fragmentPos.x() + cursorLocalPos.x();
        qreal distance = qAbs(localPos.x() - globalX);
        
        if (distance < minDistance) {
            minDistance = distance;
            result.fragment = fragment;
            result.offset = fragmentOffset;
            result.globalOffset = globalOffsetCounter + fragmentOffset;
        }
        
        globalOffsetCounter += fragment->text().length();
    }
    
    // 如果没有找到任何 fragment（理论上不应该发生），返回第一个
    if (!result.fragment) {
        for (QGraphicsItem *item : items) {
            TextFragment *fragment = qgraphicsitem_cast<TextFragment*>(item);
            if (fragment) {
                result.fragment = fragment;
                result.offset = 0;
                result.globalOffset = 0;
                break;
            }
        }
    }
    
    return result;
}

TextBlockLayoutEngine::CursorVisualResult TextBlockLayoutEngine::cursorPositionAt(int globalOffset, const QList<QGraphicsItem*> &items) const
{
    CursorVisualResult result;
    result.position = QPointF(0, 0);
    result.height = 20;
    
    qDebug() << "TextBlockLayoutEngine::cursorPositionAt - globalOffset:" << globalOffset 
             << "items count:" << items.size();
    
    if (items.isEmpty()) {
        qDebug() << "  items 为空，返回默认值";
        return result;
    }
    
    // 找到对应的 fragment 和在该 fragment 中的偏移
    TextFragment *targetFragment = nullptr;
    int offsetInFragment = 0;
    int currentOffset = 0;
    
    for (QGraphicsItem *item : items) {
        TextFragment *fragment = qgraphicsitem_cast<TextFragment*>(item);
        if (!fragment) {
            continue;
        }
        
        int fragmentLength = fragment->text().length();
        
        qDebug() << "  检查 fragment: currentOffset=" << currentOffset 
                 << "fragmentLength=" << fragmentLength 
                 << "text=" << fragment->text().left(10) << "...";
        
        if (globalOffset <= currentOffset + fragmentLength) {
            targetFragment = fragment;
            offsetInFragment = globalOffset - currentOffset;
            qDebug() << "  找到目标 fragment: offsetInFragment=" << offsetInFragment;
            break;
        }
        
        currentOffset += fragmentLength;
    }
    
    // 如果 globalOffset 超出范围，使用最后一个 fragment
    if (!targetFragment) {
        qDebug() << "  未找到目标，使用最后一个 fragment";
        for (int i = items.size() - 1; i >= 0; --i) {
            TextFragment *fragment = qgraphicsitem_cast<TextFragment*>(items[i]);
            if (fragment) {
                targetFragment = fragment;
                offsetInFragment = fragment->text().length();
                qDebug() << "  使用最后一个 fragment: offsetInFragment=" << offsetInFragment;
                break;
            }
        }
    }
    
    if (!targetFragment) {
        qDebug() << "  仍未找到 fragment，返回默认值";
        return result;
    }
    
    // 获取 fragment 的位置
    QPointF fragmentPos = positionForItem(targetFragment);
    qDebug() << "  fragment 位置:" << fragmentPos;
    
    // 获取 fragment 内的光标位置
    QPointF fragmentCursorPos = targetFragment->cursorPositionAt(offsetInFragment);
    qDebug() << "  fragment 内光标位置:" << fragmentCursorPos;
    
    // 计算相对于 TextBlockItem 的位置
    result.position = fragmentPos + fragmentCursorPos;
    result.height = targetFragment->cursorHeight();
    
    qDebug() << "  最终结果: position:" << result.position << "height:" << result.height;
    
    return result;
}

} // namespace QtWordEditor
