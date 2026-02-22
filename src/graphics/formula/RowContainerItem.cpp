#include "graphics/formula/RowContainerItem.h"
#include "graphics/factory/MathFormulaItemFactory.h"
#include <QDebug>

namespace QtWordEditor {

RowContainerItem::RowContainerItem(RowContainerMathSpan *dataSpan, QGraphicsItem *parent)
    : MathFormulaItem(dataSpan, parent)
    , m_spacing(2)
{
    if (dataSpan) {
        m_spacing = dataSpan->spacing();
    }
    createChildItems();
    relayout();
}

void RowContainerItem::relayout()
{
    if (m_data) {
        RowContainerMathSpan *containerSpan = static_cast<RowContainerMathSpan*>(m_data);
        m_spacing = containerSpan->spacing();
    }
    createChildItems();
    layoutChildren();
    update();
}

void RowContainerItem::bindData(MathSpan *data)
{
    if (data && data->mathType() == MathSpan::RowContainer) {
        m_data = data;
        RowContainerMathSpan *containerSpan = static_cast<RowContainerMathSpan*>(m_data);
        m_spacing = containerSpan->spacing();
        relayout();
    }
}

void RowContainerItem::layoutChildren()
{
    qreal x = 0;
    qreal maxBaseline = 0;
    qreal maxHeight = 0;
    
    // 找出最大基线
    for (MathFormulaItem *item : m_childItems) {
        if (item) {
            maxBaseline = qMax(maxBaseline, item->baseline());
            maxHeight = qMax(maxHeight, item->height());
        }
    }
    
    // 定位子项
    for (MathFormulaItem *item : m_childItems) {
        if (item) {
            qreal y = maxBaseline - item->baseline();
            item->setPos(x, y);
            x += item->width() + m_spacing;
        }
    }
    
    // 更新自己的尺寸
    if (!m_childItems.isEmpty()) {
        m_width = x - m_spacing; // 减去最后一个间距
    } else {
        m_width = 0;
    }
    m_height = maxHeight;
    m_baseline = maxBaseline;
    m_boundingRect = QRectF(0, 0, m_width, m_height);
}

void RowContainerItem::clearChildren()
{
    qDeleteAll(m_childItems);
    m_childItems.clear();
}

void RowContainerItem::createChildItems()
{
    clearChildren();
    
    if (!m_data) {
        return;
    }
    
    RowContainerMathSpan *containerSpan = static_cast<RowContainerMathSpan*>(m_data);
    
    for (MathSpan *childSpan : containerSpan->children()) {
        MathFormulaItem *childItem = MathFormulaItemFactory::createItem(childSpan, this);
        if (childItem) {
            m_childItems.append(childItem);
        }
    }
}

} // namespace QtWordEditor
