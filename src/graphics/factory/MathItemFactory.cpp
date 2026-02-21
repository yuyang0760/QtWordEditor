/**
 * @file MathItemFactory.cpp
 * @brief MathItem工厂类实现
 */

#include "graphics/factory/MathItemFactory.h"
#include "core/document/MathSpan.h"
#include "core/document/math/NumberMathSpan.h"
#include "core/document/math/RowContainerMathSpan.h"
#include "core/document/math/FractionMathSpan.h"
#include "graphics/formula/MathItem.h"
#include "graphics/formula/NumberItem.h"
#include "graphics/formula/RowContainerItem.h"
#include "graphics/formula/FractionItem.h"
#include <QDebug>

namespace QtWordEditor {

MathItem *MathItemFactory::createMathItem(MathSpan *span, MathItem *parent)
{
    if (!span) {
        qWarning() << "MathItemFactory::createMathItem: span is null";
        return nullptr;
    }

    switch (span->mathType()) {
    case MathSpan::Number:
        return createNumberItem(span, parent);
    case MathSpan::RowContainer:
        return createRowContainerItem(span, parent);
    case MathSpan::Fraction:
        return createFractionItem(span, parent);
    default:
        qWarning() << "MathItemFactory::createMathItem: unsupported math type:" << span->mathType();
        return nullptr;
    }
}

MathItem *MathItemFactory::createNumberItem(MathSpan *span, MathItem *parent)
{
    NumberMathSpan *numSpan = static_cast<NumberMathSpan*>(span);
    NumberItem *item = new NumberItem(numSpan, parent);
    return item;
}

MathItem *MathItemFactory::createRowContainerItem(MathSpan *span, MathItem *parent)
{
    RowContainerMathSpan *rowSpan = static_cast<RowContainerMathSpan*>(span);
    RowContainerItem *item = new RowContainerItem(rowSpan, parent);
    
    // 为每个子MathSpan创建对应的MathItem
    for (int i = 0; i < rowSpan->childCount(); ++i) {
        MathSpan *childSpan = rowSpan->childAt(i);
        MathItem *childItem = createMathItem(childSpan, item);
        if (childItem) {
            item->appendChild(childItem);
        }
    }
    
    return item;
}

MathItem *MathItemFactory::createFractionItem(MathSpan *span, MathItem *parent)
{
    FractionMathSpan *fracSpan = static_cast<FractionMathSpan*>(span);
    FractionItem *item = new FractionItem(fracSpan, parent);
    
    // 为分子创建MathItem
    MathSpan *numSpan = fracSpan->numerator();
    if (numSpan) {
        MathItem *numItem = createMathItem(numSpan, item);
        item->setNumeratorItem(numItem);
    }
    
    // 为分母创建MathItem
    MathSpan *denSpan = fracSpan->denominator();
    if (denSpan) {
        MathItem *denItem = createMathItem(denSpan, item);
        item->setDenominatorItem(denItem);
    }
    
    return item;
}

} // namespace QtWordEditor
