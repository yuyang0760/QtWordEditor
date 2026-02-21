/**
 * @file MathItem.h
 * @brief 所有公式视图元素的基类
 * 
 * 继承自QGraphicsItem，可以接收鼠标和键盘事件，实现原地编辑。
 * 每个MathItem对应一个MathSpan数据对象。
 */

#ifndef MATHITEM_H
#define MATHITEM_H

#include <QGraphicsItem>
#include <QRectF>
#include <QPainter>
#include "core/Global.h"

namespace QtWordEditor {

class MathSpan;

/**
 * @brief 所有公式视图元素的基类
 * 
 * 继承自QGraphicsItem，可以接收鼠标和键盘事件，实现原地编辑。
 * 每个MathItem对应一个MathSpan数据对象。
 */
class MathItem : public QGraphicsItem
{
public:
    /**
     * @brief QGraphicsItem 类型标识
     */
    enum { Type = UserType + 2000 };

    /**
     * @brief 构造函数
     * @param span 对应的MathSpan数据对象
     * @param parent 父MathItem（可选）
     */
    explicit MathItem(MathSpan *span, MathItem *parent = nullptr);

    /**
     * @brief 析构函数
     */
    virtual ~MathItem() = default;

    // ========== 核心布局接口 ==========

    /**
     * @brief 更新布局
     * 
     * 当数据变化时调用此方法重新计算布局。
     */
    virtual void updateLayout();

    /**
     * @brief 获取基线位置
     * @return 基线相对于item顶部的y坐标
     */
    virtual qreal baseline() const;

    // ========== 子项管理 ==========

    /**
     * @brief 在指定位置插入子元素
     * @param index 插入位置
     * @param item 要插入的子元素
     */
    virtual void insertChild(int index, MathItem *item);

    /**
     * @brief 在末尾添加子元素
     * @param item 要添加的子元素
     */
    virtual void appendChild(MathItem *item);

    /**
     * @brief 移除指定的子元素
     * @param item 要移除的子元素
     */
    virtual void removeChild(MathItem *item);

    /**
     * @brief 获取所有子元素列表
     * @return 子元素列表
     */
    virtual QList<MathItem*> children() const;

    /**
     * @brief 获取子元素数量
     * @return 子元素数量
     */
    virtual int childCount() const;

    /**
     * @brief 获取指定位置的子元素
     * @param index 位置索引
     * @return 子元素指针
     */
    virtual MathItem *childAt(int index) const;

    /**
     * @brief 获取子元素在父元素中的索引
     * @param child 子元素指针
     * @return 索引位置
     */
    virtual int indexOfChild(MathItem *child) const;

    // ========== Qt标准接口 ==========

    /**
     * @brief 获取QGraphicsItem类型
     * @return 固定返回 Type
     */
    int type() const override { return Type; }

    /**
     * @brief 获取包围盒
     * @return 包围盒矩形
     */
    QRectF boundingRect() const override = 0;

    /**
     * @brief 绘制方法
     * @param painter 绘图设备
     * @param option 样式选项
     * @param widget 控件
     */
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override = 0;

    // ========== 数据关联 ==========

    /**
     * @brief 获取对应的MathSpan数据对象
     * @return MathSpan指针
     */
    MathSpan *mathSpan() const;

    /**
     * @brief 获取父MathItem
     * @return 父MathItem指针
     */
    MathItem *parentMathItem() const;

    /**
     * @brief 判断是否是容器类型
     * @return true 表示是容器
     */
    virtual bool isContainer() const { return false; }

    // ========== 编辑支持 ==========
    
    /**
     * @brief 处理鼠标按下事件
     * @param event 鼠标事件
     */
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    
    /**
     * @brief 处理鼠标移动事件
     * @param event 鼠标事件
     */
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    
    /**
     * @brief 处理鼠标释放事件
     * @param event 鼠标事件
     */
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    
    /**
     * @brief 处理键盘按下事件
     * @param event 键盘事件
     */
    virtual void keyPressEvent(QKeyEvent *event) override;
    
    /**
     * @brief 判断一个点在公式的哪个区域
     * @param localPos 相对于自身的局部坐标
     * @return 子区域索引，对于非容器类型返回 -1
     */
    virtual int hitTestRegion(const QPointF &localPos) const;

protected:
    /**
     * @brief 通知父元素布局已变化
     */
    void notifyParentLayoutChanged();

    QRectF m_boundingRect;  ///< 包围盒
    qreal m_baseline;       ///< 基线位置
    MathSpan *m_span;       ///< 对应的MathSpan数据对象
};

} // namespace QtWordEditor

#endif // MATHITEM_H
