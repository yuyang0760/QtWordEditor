/**
 * @file RowContainerMathSpan.cpp
 * @brief 水平行容器数据类实现
 */

#include "core/document/math/RowContainerMathSpan.h"
#include <QDebug>

namespace QtWordEditor {

RowContainerMathSpan::RowContainerMathSpan(QObject *parent)
    : MathSpan(parent)
    , m_children()
    , m_spacing(2.0)
{
}

RowContainerMathSpan::~RowContainerMathSpan()
{
    // 清理所有子元素
    qDeleteAll(m_children);
    m_children.clear();
}

InlineSpan *RowContainerMathSpan::clone() const
{
    RowContainerMathSpan *cloneSpan = new RowContainerMathSpan();
    cloneSpan->setSpacing(m_spacing);
    
    // 克隆所有子元素
    for (MathSpan *child : m_children) {
        MathSpan *clonedChild = static_cast<MathSpan*>(child->clone());
        cloneSpan->appendChild(clonedChild);
    }
    
    return cloneSpan;
}

void RowContainerMathSpan::insertChild(int index, MathSpan *item)
{
    if (item) {
        // 确保 item 不在其他容器中
        if (item->parentMathSpan()) {
            item->parentMathSpan()->removeChild(item);
        }
        
        item->setParentMathSpan(this);
        m_children.insert(index, item);
        emit contentChanged();
    }
}

void RowContainerMathSpan::appendChild(MathSpan *item)
{
    if (item) {
        // 确保 item 不在其他容器中
        if (item->parentMathSpan()) {
            item->parentMathSpan()->removeChild(item);
        }
        
        item->setParentMathSpan(this);
        m_children.append(item);
        emit contentChanged();
    }
}

void RowContainerMathSpan::removeChild(MathSpan *item)
{
    if (item && m_children.contains(item)) {
        m_children.removeOne(item);
        item->setParentMathSpan(nullptr);
        emit contentChanged();
    }
}

QList<MathSpan*> RowContainerMathSpan::children() const
{
    return m_children;
}

int RowContainerMathSpan::childCount() const
{
    return m_children.size();
}

MathSpan *RowContainerMathSpan::childAt(int index) const
{
    if (index >= 0 && index < m_children.size()) {
        return m_children.at(index);
    }
    return nullptr;
}

int RowContainerMathSpan::indexOfChild(MathSpan *child) const
{
    return m_children.indexOf(child);
}

qreal RowContainerMathSpan::spacing() const
{
    return m_spacing;
}

void RowContainerMathSpan::setSpacing(qreal spacing)
{
    if (!qFuzzyCompare(m_spacing, spacing)) {
        m_spacing = spacing;
        emit contentChanged();
    }
}

} // namespace QtWordEditor
