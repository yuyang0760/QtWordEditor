/**
 * @file RowContainerItem.h
 * @brief 水平行容器视图类
 * 
 * 用于渲染RowContainerMathSpan数据，在水平方向上排列多个公式元素。
 */

#ifndef ROWCONTAINERITEM_H
#define ROWCONTAINERITEM_H

#include "graphics/formula/MathItem.h"
#include <QList>
#include <QPointF>
#include "core/Global.h"

namespace QtWordEditor {

class RowContainerMathSpan;

/**
 * @brief 水平行容器视图类
 * 
 * 用于渲染RowContainerMathSpan数据，在水平方向上排列多个公式元素。
 */
class RowContainerItem : public MathItem
{
public:
    /**
     * @brief QGraphicsItem 类型标识
     */
    enum { Type = UserType + 2001 };

    /**
     * @brief 构造函数
     * @param span 对应的RowContainerMathSpan数据对象
     * @param parent 父MathItem（可选）
     */
    RowContainerItem(RowContainerMathSpan *span, MathItem *parent = nullptr);

    /**
     * @brief 析构函数
     */
    ~RowContainerItem() override;

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

    // ========== 子项管理（重写） ==========

    /**
     * @brief 在指定位置插入子元素
     * @param index 插入位置
     * @param item 要插入的子元素
     */
    void insertChild(int index, MathItem *item) override;

    /**
     * @brief 在末尾添加子元素
     * @param item 要添加的子元素
     */
    void appendChild(MathItem *item) override;

    /**
     * @brief 移除指定的子元素
     * @param item 要移除的子元素
     */
    void removeChild(MathItem *item) override;

    /**
     * @brief 获取所有子元素列表
     * @return 子元素列表
     */
    QList<MathItem*> children() const override;

    /**
     * @brief 获取子元素数量
     * @return 子元素数量
     */
    int childCount() const override;

    /**
     * @brief 获取指定位置的子元素
     * @param index 位置索引
     * @return 子元素指针
     */
    MathItem *childAt(int index) const override;

    /**
     * @brief 获取子元素在父元素中的索引
     * @param child 子元素指针
     * @return 索引位置
     */
    int indexOfChild(MathItem *child) const override;

    // ========== RowContainerItem 特有接口 ==========

    /**
     * @brief 判断是否是容器类型
     * @return 固定返回 true
     */
    bool isContainer() const override { return true; }

    /**
     * @brief 获取对应的RowContainerMathSpan
     * @return RowContainerMathSpan指针
     */
    RowContainerMathSpan *rowSpan() const;
    
    /**
     * @brief 获取指定位置的光标 X 坐标
     * @param position 位置索引（0 到 childCount()）
     * @return 光标 X 坐标（相对于 RowContainerItem）
     */
    qreal cursorXAt(int position) const;

private:
    QList<MathItem*> m_children;       ///< 子元素列表
    QList<QPointF> m_childPositions;   ///< 子元素位置
    qreal m_spacing;                    ///< 子元素之间的间距
};

} // namespace QtWordEditor

#endif // ROWCONTAINERITEM_H
