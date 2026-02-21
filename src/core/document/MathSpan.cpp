/**
 * @file MathSpan.cpp
 * @brief 公式内联元素数据基类实现
 */

#include "core/document/MathSpan.h"
#include <QDebug>

namespace QtWordEditor {

MathSpan::MathSpan(QObject *parent)
    : InlineSpan(parent)
    , m_parent(nullptr)
{
}

MathSpan::~MathSpan()
{
    // 清理子元素（由具体的容器类负责）
}

void MathSpan::insertChild(int index, MathSpan *item)
{
    Q_UNUSED(index);
    Q_UNUSED(item);
    qWarning() << "MathSpan::insertChild: Not a container type";
}

void MathSpan::appendChild(MathSpan *item)
{
    Q_UNUSED(item);
    qWarning() << "MathSpan::appendChild: Not a container type";
}

void MathSpan::removeChild(MathSpan *item)
{
    Q_UNUSED(item);
    qWarning() << "MathSpan::removeChild: Not a container type";
}

QList<MathSpan*> MathSpan::children() const
{
    return QList<MathSpan*>();
}

int MathSpan::childCount() const
{
    return 0;
}

MathSpan *MathSpan::childAt(int index) const
{
    Q_UNUSED(index);
    return nullptr;
}

int MathSpan::indexOfChild(MathSpan *child) const
{
    Q_UNUSED(child);
    return -1;
}

MathSpan *MathSpan::parentMathSpan() const
{
    return m_parent;
}

void MathSpan::setParentMathSpan(MathSpan *parent)
{
    m_parent = parent;
}

} // namespace QtWordEditor
