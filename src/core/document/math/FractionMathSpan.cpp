/**
 * @file FractionMathSpan.cpp
 * @brief 分数数据类实现
 */

#include "core/document/math/FractionMathSpan.h"

namespace QtWordEditor {

FractionMathSpan::FractionMathSpan(QObject *parent)
    : MathSpan(parent)
    , m_numerator(nullptr)
    , m_denominator(nullptr)
{
}

FractionMathSpan::FractionMathSpan(MathSpan *numerator, MathSpan *denominator, QObject *parent)
    : MathSpan(parent)
    , m_numerator(nullptr)
    , m_denominator(nullptr)
{
    setNumerator(numerator);
    setDenominator(denominator);
}

FractionMathSpan::~FractionMathSpan()
{
    // 清理分子和分母
    if (m_numerator) {
        delete m_numerator;
        m_numerator = nullptr;
    }
    if (m_denominator) {
        delete m_denominator;
        m_denominator = nullptr;
    }
}

InlineSpan *FractionMathSpan::clone() const
{
    MathSpan *clonedNumerator = nullptr;
    MathSpan *clonedDenominator = nullptr;
    
    if (m_numerator) {
        clonedNumerator = static_cast<MathSpan*>(m_numerator->clone());
    }
    if (m_denominator) {
        clonedDenominator = static_cast<MathSpan*>(m_denominator->clone());
    }
    
    return new FractionMathSpan(clonedNumerator, clonedDenominator);
}

QList<MathSpan*> FractionMathSpan::children() const
{
    QList<MathSpan*> list;
    if (m_numerator) {
        list.append(m_numerator);
    }
    if (m_denominator) {
        list.append(m_denominator);
    }
    return list;
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
    if (index == 0) return m_numerator;
    if (index == 1) return m_denominator;
    return nullptr;
}

MathSpan *FractionMathSpan::numerator() const
{
    return m_numerator;
}

void FractionMathSpan::setNumerator(MathSpan *numerator)
{
    if (m_numerator != numerator) {
        // 清理旧的分子
        if (m_numerator) {
            m_numerator->setParentMathSpan(nullptr);
            delete m_numerator;
        }
        
        // 设置新的分子
        m_numerator = numerator;
        if (m_numerator) {
            // 确保不在其他容器中
            if (m_numerator->parentMathSpan()) {
                m_numerator->parentMathSpan()->removeChild(m_numerator);
            }
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
        // 清理旧的分母
        if (m_denominator) {
            m_denominator->setParentMathSpan(nullptr);
            delete m_denominator;
        }
        
        // 设置新的分母
        m_denominator = denominator;
        if (m_denominator) {
            // 确保不在其他容器中
            if (m_denominator->parentMathSpan()) {
                m_denominator->parentMathSpan()->removeChild(m_denominator);
            }
            m_denominator->setParentMathSpan(this);
        }
        
        emit contentChanged();
    }
}

} // namespace QtWordEditor
