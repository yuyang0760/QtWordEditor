/**
 * @file RowContainerItem.cpp
 * @brief 水平行容器视图类实现
 */

#include "graphics/formula/RowContainerItem.h"
#include "core/document/math/RowContainerMathSpan.h"
#include <QPainter>

namespace QtWordEditor {

RowContainerItem::RowContainerItem(RowContainerMathSpan *span, MathItem *parent)
    : MathItem(span, parent)
    , m_children()
    , m_childPositions()
    , m_spacing(2.0)
{
    if (span) {
        m_spacing = span->spacing();
    }
}

RowContainerItem::~RowContainerItem()
{
    // 子元素由QGraphicsItem的父子关系管理
}

void RowContainerItem::updateLayout()
{
    RowContainerMathSpan *rowSpan = this->rowSpan();
    if (!rowSpan) return;

    m_spacing = rowSpan->spacing();

    qreal totalWidth = 0;
    qreal maxAscent = 0;
    qreal maxDescent = 0;

    m_childPositions.clear();

    // 计算所有子元素的位置
    for (MathItem *child : m_children) {
        child->updateLayout();
        
        QPointF pos(totalWidth, 0);
        m_childPositions.append(pos);
        
        // 设置子元素的位置
        child->setPos(pos);
        
        totalWidth += child->boundingRect().width();
        
        // 计算最大的ascent和descent
        qreal childBaseline = child->baseline();
        qreal childHeight = child->boundingRect().height();
        qreal childDescent = childHeight - childBaseline;
        
        if (childBaseline > maxAscent) {
            maxAscent = childBaseline;
        }
        if (childDescent > maxDescent) {
            maxDescent = childDescent;
        }
        
        // 添加间距（除了最后一个元素）
        if (child != m_children.last()) {
            totalWidth += m_spacing;
        }
    }

    // 调整所有子元素的垂直位置，使基线对齐
    for (int i = 0; i < m_children.size(); ++i) {
        MathItem *child = m_children[i];
        qreal childBaseline = child->baseline();
        qreal yOffset = maxAscent - childBaseline;
        
        QPointF pos = m_childPositions[i];
        pos.setY(yOffset);
        m_childPositions[i] = pos;
        child->setPos(pos);
    }

    qreal totalHeight = maxAscent + maxDescent;
    m_boundingRect = QRectF(0, 0, totalWidth, totalHeight);
    m_baseline = maxAscent;

    notifyParentLayoutChanged();
}

qreal RowContainerItem::baseline() const
{
    return m_baseline;
}

QRectF RowContainerItem::boundingRect() const
{
    return m_boundingRect;
}

void RowContainerItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    Q_UNUSED(painter);
    
    // 子元素由QGraphicsItem自动绘制
}

void RowContainerItem::insertChild(int index, MathItem *item)
{
    if (item) {
        item->setParentItem(this);
        m_children.insert(index, item);
        updateLayout();
    }
}

void RowContainerItem::appendChild(MathItem *item)
{
    if (item) {
        item->setParentItem(this);
        m_children.append(item);
        updateLayout();
    }
}

void RowContainerItem::removeChild(MathItem *item)
{
    if (item && m_children.contains(item)) {
        m_children.removeOne(item);
        item->setParentItem(nullptr);
        updateLayout();
    }
}

QList<MathItem*> RowContainerItem::children() const
{
    return m_children;
}

int RowContainerItem::childCount() const
{
    return m_children.size();
}

MathItem *RowContainerItem::childAt(int index) const
{
    if (index >= 0 && index < m_children.size()) {
        return m_children.at(index);
    }
    return nullptr;
}

int RowContainerItem::indexOfChild(MathItem *child) const
{
    return m_children.indexOf(child);
}

RowContainerMathSpan *RowContainerItem::rowSpan() const
{
    return static_cast<RowContainerMathSpan*>(m_span);
}

} // namespace QtWordEditor
