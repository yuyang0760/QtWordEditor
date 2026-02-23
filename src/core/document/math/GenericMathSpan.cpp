/**
 * @file GenericMathSpan.cpp
 * @brief 通用公式容器数据类（简化版）实现
 */

#include "core/document/math/GenericMathSpan.h"
#include <QDebug>

namespace QtWordEditor {

GenericMathSpan::GenericMathSpan(QObject *parent)
    : MathSpan(parent)
    , m_spans()
{
}

GenericMathSpan::~GenericMathSpan()
{
    // InlineSpan 由 QObject 父子关系管理
}

InlineSpan *GenericMathSpan::clone() const
{
    GenericMathSpan *newSpan = new GenericMathSpan();
    // 注意：这里只做浅拷贝，因为 InlineSpan 的克隆需要更复杂的处理
    newSpan->m_spans = m_spans;
    return newSpan;
}

QList<InlineSpan*> GenericMathSpan::spans() const
{
    return m_spans;
}

int GenericMathSpan::spanCount() const
{
    return m_spans.size();
}

InlineSpan *GenericMathSpan::spanAt(int index) const
{
    if (index >= 0 && index < m_spans.size()) {
        return m_spans.at(index);
    }
    return nullptr;
}

void GenericMathSpan::insertSpan(int index, InlineSpan *span)
{
    if (!span || index < 0 || index > m_spans.size()) {
        return;
    }
    span->setParent(this);
    m_spans.insert(index, span);
    emit spansChanged();
    emit contentChanged();
}

void GenericMathSpan::appendSpan(InlineSpan *span)
{
    insertSpan(m_spans.size(), span);
}

void GenericMathSpan::removeSpanAt(int index)
{
    if (index < 0 || index >= m_spans.size()) {
        return;
    }
    InlineSpan *span = m_spans.takeAt(index);
    span->setParent(nullptr);
    emit spansChanged();
    emit contentChanged();
}

void GenericMathSpan::clearSpans()
{
    for (InlineSpan *span : m_spans) {
        span->setParent(nullptr);
    }
    m_spans.clear();
    emit spansChanged();
    emit contentChanged();
}

} // namespace QtWordEditor
