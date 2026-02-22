#include "graphics/factory/MathFormulaItemFactory.h"
#include "graphics/formula/NumberItem.h"
#include "graphics/formula/RowContainerItem.h"
#include "graphics/formula/FractionItem.h"
#include "core/document/math/NumberMathSpan.h"
#include "core/document/math/RowContainerMathSpan.h"
#include "core/document/math/FractionMathSpan.h"
#include <QDebug>

namespace QtWordEditor {

MathFormulaItemFactory::MathFormulaItemFactory()
{
}

MathFormulaItem *MathFormulaItemFactory::createItem(MathSpan *data, QGraphicsItem *parent)
{
    if (!data) {
        return nullptr;
    }

    switch (data->mathType()) {
        case MathSpan::Number:
            return new NumberItem(static_cast<NumberMathSpan*>(data), parent);

        case MathSpan::RowContainer:
            return new RowContainerItem(static_cast<RowContainerMathSpan*>(data), parent);

        case MathSpan::Fraction:
            return new FractionItem(static_cast<FractionMathSpan*>(data), parent);

        // TODO: 实现其他类型的公式项
        default:
            qWarning() << "MathFormulaItemFactory::createItem - 未处理的公式类型:" << static_cast<int>(data->mathType());
            return nullptr;
    }
}

} // namespace QtWordEditor
