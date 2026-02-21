/**
 * @file MathItemFactory.h
 * @brief MathItem工厂类
 * 
 * 用于根据MathSpan类型创建对应的MathItem视图对象。
 */

#ifndef MATHITEMFACTORY_H
#define MATHITEMFACTORY_H

#include "core/Global.h"

namespace QtWordEditor {

class MathSpan;
class MathItem;

/**
 * @brief MathItem工厂类
 * 
 * 用于根据MathSpan类型创建对应的MathItem视图对象。
 */
class MathItemFactory
{
public:
    /**
     * @brief 根据MathSpan创建对应的MathItem
     * @param span MathSpan数据对象
     * @param parent 父MathItem（可选）
     * @return 创建的MathItem指针，失败返回nullptr
     */
    static MathItem *createMathItem(MathSpan *span, MathItem *parent = nullptr);

private:
    /**
     * @brief 创建NumberItem
     * @param span NumberMathSpan数据对象
     * @param parent 父MathItem
     * @return NumberItem指针
     */
    static MathItem *createNumberItem(MathSpan *span, MathItem *parent);

    /**
     * @brief 创建RowContainerItem
     * @param span RowContainerMathSpan数据对象
     * @param parent 父MathItem
     * @return RowContainerItem指针
     */
    static MathItem *createRowContainerItem(MathSpan *span, MathItem *parent);

    /**
     * @brief 创建FractionItem
     * @param span FractionMathSpan数据对象
     * @param parent 父MathItem
     * @return FractionItem指针
     */
    static MathItem *createFractionItem(MathSpan *span, MathItem *parent);
};

} // namespace QtWordEditor

#endif // MATHITEMFACTORY_H
