#ifndef MATHFORMULAITEMFACTORY_H
#define MATHFORMULAITEMFACTORY_H

#include "core/document/MathSpan.h"
#include "graphics/formula/MathFormulaItem.h"
#include <QGraphicsItem>
#include "core/Global.h"

namespace QtWordEditor {

/**
 * @brief 公式图形项工厂类
 * 
 * 负责根据数据类型创建对应的公式图形项
 */
class MathFormulaItemFactory
{
public:
    MathFormulaItemFactory();
    ~MathFormulaItemFactory() = default;

    /**
     * @brief 根据数据创建公式图形项
     * @param data 公式数据
     * @param parent 父项
     * @return 公式图形项
     */
    static MathFormulaItem *createItem(MathSpan *data, QGraphicsItem *parent = nullptr);
};

} // namespace QtWordEditor

#endif // MATHFORMULAITEMFACTORY_H
