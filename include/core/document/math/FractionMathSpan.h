#ifndef FRACTIONMATHSPAN_H
#define FRACTIONMATHSPAN_H

#include "core/document/MathSpan.h"
#include "core/Global.h"

namespace QtWordEditor {

class FractionMathSpan : public MathSpan
{
    Q_OBJECT
public:
    FractionMathSpan(QObject *parent = nullptr);
    FractionMathSpan(MathSpan *numerator, MathSpan *denominator, QObject *parent = nullptr);
    ~FractionMathSpan() override;

    MathType mathType() const override { return Fraction; }
    bool isContainer() const override { return true; }
    InlineSpan *clone() const override;

    QList<MathSpan*> children() const override;
    int childCount() const override;
    MathSpan *childAt(int index) const override;

    MathSpan *numerator() const;
    void setNumerator(MathSpan *numerator);
    MathSpan *denominator() const;
    void setDenominator(MathSpan *denominator);

private:
    MathSpan *m_numerator;
    MathSpan *m_denominator;
};

} // namespace QtWordEditor

#endif // FRACTIONMATHSPAN_H
