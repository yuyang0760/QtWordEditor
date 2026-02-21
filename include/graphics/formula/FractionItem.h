/**
 * @file FractionItem.h
 * @brief 分数视图类
 * 
 * 用于渲染FractionMathSpan数据，显示分数公式。
 */

#ifndef FRACTIONITEM_H
#define FRACTIONITEM_H

#include "graphics/formula/MathItem.h"
#include <QPointF>
#include "core/Global.h"

namespace QtWordEditor {

class FractionMathSpan;

/**
 * @brief 分数视图类
 * 
 * 用于渲染FractionMathSpan数据，显示分数公式。
 */
class FractionItem : public MathItem
{
public:
    /**
     * @brief QGraphicsItem 类型标识
     */
    enum { Type = UserType + 2003 };

    /**
     * @brief 构造函数
     * @param span 对应的FractionMathSpan数据对象
     * @param parent 父MathItem（可选）
     */
    FractionItem(FractionMathSpan *span, MathItem *parent = nullptr);

    /**
     * @brief 析构函数
     */
    ~FractionItem() override;

    // ========== MathItem 接口实现 ==========

    /**
     * @brief 更新布局
     */
    void updateLayout() override;

    /**
     * @brief 获取基线位置
     * @return 基线位置（分数线位置）
     */
    qreal baseline() const override;

    /**
     * @brief 获取QGraphicsItem类型
     * @return 固定返回 Type
     */
    int type() const override { return Type; }

    /**
     * @brief 获取包围盒
     * @return 包围盒矩形
     */
    QRectF boundingRect() const override;

    /**
     * @brief 绘制方法
     * @param painter 绘图设备
     * @param option 样式选项
     * @param widget 控件
     */
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    // ========== 子项管理（重写） ==========

    /**
     * @brief 获取所有子元素列表（分子和分母）
     * @return 子元素列表 [分子, 分母]
     */
    QList<MathItem*> children() const override;

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
    MathItem *childAt(int index) const override;

    // ========== FractionItem 特有接口 ==========

    /**
     * @brief 判断是否是容器类型
     * @return 固定返回 true
     */
    bool isContainer() const override { return true; }

    /**
     * @brief 获取对应的FractionMathSpan
     * @return FractionMathSpan指针
     */
    FractionMathSpan *fractionSpan() const;

    /**
     * @brief 获取分子视图
     * @return 分子MathItem指针
     */
    MathItem *numeratorItem() const;

    /**
     * @brief 设置分子视图
     * @param item 分子MathItem
     */
    void setNumeratorItem(MathItem *item);

    /**
     * @brief 获取分母视图
     * @return 分母MathItem指针
     */
    MathItem *denominatorItem() const;

    /**
     * @brief 设置分母视图
     * @param item 分母MathItem
     */
    void setDenominatorItem(MathItem *item);
    
    /**
     * @brief 判断一个点在分数的哪个区域
     * @param localPos 相对于FractionItem的局部坐标
     * @return 0=分子区域，1=分母区域，-1=其他区域
     */
    int hitTestRegion(const QPointF &localPos) const;

    // ========== 位置访问 ==========

    /**
     * @brief 获取分子位置
     * @return 分子位置（相对于 FractionItem）
     */
    QPointF numeratorPos() const;

    /**
     * @brief 获取分母位置
     * @return 分母位置（相对于 FractionItem）
     */
    QPointF denominatorPos() const;

private:
    MathItem *m_numeratorItem;   ///< 分子视图
    MathItem *m_denominatorItem; ///< 分母视图
    QPointF m_numPos;            ///< 分子位置
    QPointF m_denPos;            ///< 分母位置
    qreal m_lineY;               ///< 分数线Y位置
    qreal m_lineThickness;       ///< 分数线厚度
    qreal m_padding;             ///< 内边距
};

} // namespace QtWordEditor

#endif // FRACTIONITEM_H
