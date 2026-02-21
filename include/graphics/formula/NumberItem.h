/**
 * @file NumberItem.h
 * @brief 数字/变量视图类
 * 
 * 用于渲染NumberMathSpan数据，显示数字、变量、标识符等文本内容。
 */

#ifndef NUMBERITEM_H
#define NUMBERITEM_H

#include "graphics/formula/MathItem.h"
#include <QString>
#include <QFont>
#include "core/Global.h"

namespace QtWordEditor {

class NumberMathSpan;

/**
 * @brief 数字/变量视图类
 * 
 * 用于渲染NumberMathSpan数据，显示数字、变量、标识符等文本内容。
 */
class NumberItem : public MathItem
{
public:
    /**
     * @brief QGraphicsItem 类型标识
     */
    enum { Type = UserType + 2002 };

    /**
     * @brief 构造函数
     * @param span 对应的NumberMathSpan数据对象
     * @param parent 父MathItem（可选）
     */
    NumberItem(NumberMathSpan *span, MathItem *parent = nullptr);

    /**
     * @brief 析构函数
     */
    ~NumberItem() override;

    // ========== MathItem 接口实现 ==========

    /**
     * @brief 更新布局
     */
    void updateLayout() override;

    /**
     * @brief 获取基线位置
     * @return 基线位置
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

    // ========== NumberItem 特有接口 ==========

    /**
     * @brief 获取对应的NumberMathSpan
     * @return NumberMathSpan指针
     */
    NumberMathSpan *numberSpan() const;

    // ========== 字符位置计算 ==========

    /**
     * @brief 获取指定位置的光标 X 坐标
     * @param position 位置索引（0 到 textLength()）
     * @return 光标 X 坐标（相对于 NumberItem）
     */
    qreal cursorXAt(int position) const;

    /**
     * @brief 获取文本长度
     * @return 文本长度
     */
    int textLength() const;

    /**
     * @brief 根据局部 X 坐标获取字符位置
     * @param x 局部 X 坐标
     * @return 字符位置索引
     */
    int hitTestX(qreal x) const;

private:
    QFont m_font;  ///< 用于渲染文本的字体
};

} // namespace QtWordEditor

#endif // NUMBERITEM_H
