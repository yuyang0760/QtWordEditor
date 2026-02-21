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
    qDebug() << "  [MathItemFactory::createMathItem] 开始, span=" << span << ", parent=" << parent;
    if (!span) {
        qWarning() << "MathItemFactory::createMathItem: span is null";
        return nullptr;
    }

    qDebug() << "  [MathItemFactory::createMathItem] mathType=" << (int)span->mathType();
    
    switch (span->mathType()) {
    case MathSpan::Number:
        qDebug() << "  [MathItemFactory::createMathItem] 准备调用 createNumberItem...";
        return createNumberItem(span, parent);
    case MathSpan::RowContainer:
        qDebug() << "  [MathItemFactory::createMathItem] 准备调用 createRowContainerItem...";
        return createRowContainerItem(span, parent);
    case MathSpan::Fraction:
        qDebug() << "  [MathItemFactory::createMathItem] 准备调用 createFractionItem...";
        return createFractionItem(span, parent);
    default:
        qWarning() << "MathItemFactory::createMathItem: unsupported math type:" << span->mathType();
        return nullptr;
    }
}

MathItem *MathItemFactory::createNumberItem(MathSpan *span, MathItem *parent)
{
    qDebug() << "    [MathItemFactory::createNumberItem] 开始...";
    NumberMathSpan *numSpan = static_cast<NumberMathSpan*>(span);
    qDebug() << "    [MathItemFactory::createNumberItem] numSpan=" << numSpan;
    NumberItem *item = new NumberItem(numSpan, parent);
    qDebug() << "    [MathItemFactory::createNumberItem] NumberItem 创建成功, item=" << item;
    return item;
}

MathItem *MathItemFactory::createRowContainerItem(MathSpan *span, MathItem *parent)
{
    qDebug() << "    [MathItemFactory::createRowContainerItem] 开始...";
    RowContainerMathSpan *rowSpan = static_cast<RowContainerMathSpan*>(span);
    qDebug() << "    [MathItemFactory::createRowContainerItem] rowSpan=" << rowSpan;
    RowContainerItem *item = new RowContainerItem(rowSpan, parent);
    qDebug() << "    [MathItemFactory::createRowContainerItem] RowContainerItem 创建成功";
    
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
    qDebug() << "    [MathItemFactory::createFractionItem] 开始...";
    FractionMathSpan *fracSpan = static_cast<FractionMathSpan*>(span);
    qDebug() << "    [MathItemFactory::createFractionItem] fracSpan=" << fracSpan;
    FractionItem *item = new FractionItem(fracSpan, parent);
    qDebug() << "    [MathItemFactory::createFractionItem] FractionItem 创建成功";
    
    // 为分子创建MathItem
    MathSpan *numSpan = fracSpan->numerator();
    qDebug() << "    [MathItemFactory::createFractionItem] numerator=" << numSpan;
    if (numSpan) {
        qDebug() << "    [MathItemFactory::createFractionItem] 准备为分子创建 MathItem...";
        MathItem *numItem = createMathItem(numSpan, item);
        qDebug() << "    [MathItemFactory::createFractionItem] 分子 MathItem 创建成功, numItem=" << numItem;
        item->setNumeratorItem(numItem);
        qDebug() << "    [MathItemFactory::createFractionItem] setNumeratorItem 完成";
    }
    
    // 为分母创建MathItem
    MathSpan *denSpan = fracSpan->denominator();
    qDebug() << "    [MathItemFactory::createFractionItem] denominator=" << denSpan;
    if (denSpan) {
        qDebug() << "    [MathItemFactory::createFractionItem] 准备为分母创建 MathItem...";
        MathItem *denItem = createMathItem(denSpan, item);
        qDebug() << "    [MathItemFactory::createFractionItem] 分母 MathItem 创建成功, denItem=" << denItem;
        item->setDenominatorItem(denItem);
        qDebug() << "    [MathItemFactory::createFractionItem] setDenominatorItem 完成";
    }
    
    qDebug() << "    [MathItemFactory::createFractionItem] 返回";
    return item;
}

} // namespace QtWordEditor
