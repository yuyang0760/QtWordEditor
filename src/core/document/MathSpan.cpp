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
}

void MathSpan::insertChild(int index, MathSpan *item)
{
    Q_UNUSED(index);
    Q_UNUSED(item);
    qWarning() << "MathSpan::insertChild - 非容器类不能插入子项";
}

void MathSpan::appendChild(MathSpan *item)
{
    Q_UNUSED(item);
    qWarning() << "MathSpan::appendChild - 非容器类不能添加子项";
}

void MathSpan::removeChild(MathSpan *item)
{
    Q_UNUSED(item);
    qWarning() << "MathSpan::removeChild - 非容器类不能移除子项";
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
