#ifndef FRACTIONITEM_H
#define FRACTIONITEM_H

#include "graphics/formula/MathFormulaItem.h"
#include "core/document/math/FractionMathSpan.h"
#include "core/Global.h"

namespace QtWordEditor {

class FractionItem : public MathFormulaItem
{
public:
    FractionItem(FractionMathSpan *dataSpan, QGraphicsItem *parent = nullptr);
    ~FractionItem() override = default;

    void relayout() override;
    void bindData(MathSpan *data) override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    void layoutNumeratorAndDenominator();
    void clearChildren();
    void createChildItems();
    void calculateSize();
    void updateBoundingRect();

    MathFormulaItem *m_numeratorItem;
    MathFormulaItem *m_denominatorItem;
    qreal m_lineThickness;
    qreal m_gap;
};

} // namespace QtWordEditor

#endif // FRACTIONITEM_H
