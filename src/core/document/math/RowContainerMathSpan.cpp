#include "core/document/math/RowContainerMathSpan.h"
#include <QDebug>

namespace QtWordEditor {

RowContainerMathSpan::RowContainerMathSpan(QObject *parent)
    : MathSpan(parent)
    , m_spacing(2)
{
}

RowContainerMathSpan::~RowContainerMathSpan()
{
    // 清理子项
    qDeleteAll(m_children);
    m_children.clear();
}

void RowContainerMathSpan::insertChild(int index, MathSpan *item)
{
    if (item) {
        item->setParentMathSpan(this);
        m_children.insert(index, item);
        emit contentChanged();
    }
}

void RowContainerMathSpan::appendChild(MathSpan *item)
{
    if (item) {
        item->setParentMathSpan(this);
        m_children.append(item);
        emit contentChanged();
    }
}

void RowContainerMathSpan::removeChild(MathSpan *item)
{
    if (item) {
        int index = m_children.indexOf(item);
        if (index != -1) {
            m_children.removeAt(index);
            item->setParentMathSpan(nullptr);
            emit contentChanged();
        }
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
    if (m_spacing != spacing) {
        m_spacing = spacing;
        emit contentChanged();
    }
}

InlineSpan *RowContainerMathSpan::clone() const
{
    RowContainerMathSpan *copy = new RowContainerMathSpan(nullptr);
    copy->setSpacing(m_spacing);
    
    // 克隆所有子项
    for (MathSpan *child : m_children) {
        MathSpan *childCopy = static_cast<MathSpan*>(child->clone());
        copy->appendChild(childCopy);
    }
    
    return copy;
}

} // namespace QtWordEditor
