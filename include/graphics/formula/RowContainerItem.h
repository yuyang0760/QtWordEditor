#ifndef ROWCONTAINERITEM_H
#define ROWCONTAINERITEM_H

#include "graphics/formula/MathFormulaItem.h"
#include "core/document/math/RowContainerMathSpan.h"
#include <QList>
#include "core/Global.h"

namespace QtWordEditor {

class RowContainerItem : public MathFormulaItem
{
public:
    RowContainerItem(RowContainerMathSpan *dataSpan, QGraphicsItem *parent = nullptr);
    ~RowContainerItem() override = default;

    void relayout() override;
    void bindData(MathSpan *data) override;

private:
    void layoutChildren();
    void clearChildren();
    void createChildItems();

    QList<MathFormulaItem*> m_childItems;
    qreal m_spacing;
};

} // namespace QtWordEditor

#endif // ROWCONTAINERITEM_H
