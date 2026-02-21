/**
 * @file FractionMathSpan.h
 * @brief 分数数据类
 * 
 * 用于表示分数公式，包含分子和分母两个子元素。
 */

#ifndef FRACTIONMATHSPAN_H
#define FRACTIONMATHSPAN_H

#include "core/document/MathSpan.h"
#include "core/Global.h"

namespace QtWordEditor {

/**
 * @brief 分数数据类
 * 
 * 用于表示分数公式，包含分子和分母两个子元素。
 */
class FractionMathSpan : public MathSpan
{
    Q_OBJECT
public:
    /**
     * @brief 默认构造函数
     */
    explicit FractionMathSpan(QObject *parent = nullptr);

    /**
     * @brief 带分子和分母的构造函数
     * @param numerator 分子元素
     * @param denominator 分母元素
     */
    FractionMathSpan(MathSpan *numerator, MathSpan *denominator, QObject *parent = nullptr);

    /**
     * @brief 析构函数
     */
    ~FractionMathSpan() override;

    // ========== MathSpan 接口实现 ==========

    /**
     * @brief 获取公式元素类型
     * @return 固定返回 MathSpan::Fraction
     */
    MathType mathType() const override { return Fraction; }

    /**
     * @brief 判断是否是容器类型
     * @return 固定返回 true
     */
    bool isContainer() const override { return true; }

    /**
     * @brief 克隆当前对象
     * @return 新的 FractionMathSpan 指针
     */
    InlineSpan *clone() const override;

    // ========== 子项管理（重写） ==========

    /**
     * @brief 获取所有子元素列表（分子和分母）
     * @return 子元素列表 [分子, 分母]
     */
    QList<MathSpan*> children() const override;

    /**
     * @brief 获取子元素数量（固定为2）
     * @return 固定返回 2
     */
    int childCount() const override;

    /**
     * @brief 获取指定位置的子元素
     * @param index 0=分子, 1=分母
     * @return 子元素指针
     */
    MathSpan *childAt(int index) const override;

    // ========== FractionMathSpan 特有接口 ==========

    /**
     * @brief 获取分子元素
     * @return 分子元素指针
     */
    MathSpan *numerator() const;

    /**
     * @brief 设置分子元素
     * @param numerator 新的分子元素
     */
    void setNumerator(MathSpan *numerator);

    /**
     * @brief 获取分母元素
     * @return 分母元素指针
     */
    MathSpan *denominator() const;

    /**
     * @brief 设置分母元素
     * @param denominator 新的分母元素
     */
    void setDenominator(MathSpan *denominator);

private:
    MathSpan *m_numerator;    ///< 分子元素
    MathSpan *m_denominator;  ///< 分母元素
};

} // namespace QtWordEditor

#endif // FRACTIONMATHSPAN_H
