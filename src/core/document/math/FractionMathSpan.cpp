#include "core/document/math/FractionMathSpan.h"
#include <QDebug>

namespace QtWordEditor {

FractionMathSpan::FractionMathSpan(QObject *parent)
    : MathSpan(parent)
    , m_numerator(nullptr)
    , m_denominator(nullptr)
{
}

FractionMathSpan::FractionMathSpan(MathSpan *numerator, MathSpan *denominator, QObject *parent)
    : MathSpan(parent)
    , m_numerator(numerator)
    , m_denominator(denominator)
{
    if (m_numerator) {
        m_numerator->setParentMathSpan(this);
    }
    if (m_denominator) {
        m_denominator->setParentMathSpan(this);
    }
}

FractionMathSpan::~FractionMathSpan()
{
    // 清理分子和分母
    delete m_numerator;
    delete m_denominator;
}

MathSpan *FractionMathSpan::numerator() const
{
    return m_numerator;
}

void FractionMathSpan::setNumerator(MathSpan *numerator)
{
    if (m_numerator != numerator) {
        delete m_numerator;
        m_numerator = numerator;
        if (m_numerator) {
            m_numerator->setParentMathSpan(this);
        }
        emit contentChanged();
    }
}

MathSpan *FractionMathSpan::denominator() const
{
    return m_denominator;
}

void FractionMathSpan::setDenominator(MathSpan *denominator)
{
    if (m_denominator != denominator) {
        delete m_denominator;
        m_denominator = denominator;
        if (m_denominator) {
            m_denominator->setParentMathSpan(this);
        }
        emit contentChanged();
    }
}

QList<MathSpan*> FractionMathSpan::children() const
{
    QList<MathSpan*> result;
    if (m_numerator) {
        result.append(m_numerator);
    }
    if (m_denominator) {
        result.append(m_denominator);
    }
    return result;
}

int FractionMathSpan::childCount() const
{
    int count = 0;
    if (m_numerator) count++;
    if (m_denominator) count++;
    return count;
}

MathSpan *FractionMathSpan::childAt(int index) const
{
    if (index == 0 && m_numerator) {
        return m_numerator;
    } else if (index == 1 && m_denominator) {
        return m_denominator;
    }
    return nullptr;
}

InlineSpan *FractionMathSpan::clone() const
{
    MathSpan *newNumerator = nullptr;
    MathSpan *newDenominator = nullptr;
    
    if (m_numerator) {
        newNumerator = static_cast<MathSpan*>(m_numerator->clone());
    }
    if (m_denominator) {
        newDenominator = static_cast<MathSpan*>(m_denominator->clone());
    }
    
    FractionMathSpan *copy = new FractionMathSpan(newNumerator, newDenominator, nullptr);
    return copy;
}

} // namespace QtWordEditor
