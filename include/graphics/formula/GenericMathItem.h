/**
 * @file GenericMathItem.h
 * @brief 通用公式容器视图类（简化版）
 * 
 * 使用 TextBlockLayoutEngine，类似 TextBlockItem 的设计，
 * 但配置为 NoWrap 模式，没有段落、缩进等复杂功能。
 * 专门用于公式内部的文本和公式混排。
 */

#ifndef GENERICMATHITEM_H
#define GENERICMATHITEM_H

#include "graphics/formula/MathItem.h"
#include "graphics/items/TextBlockLayoutEngine.h"
#include <QList>
#include <QRectF>
#include <QPointF>
#include "core/Global.h"

namespace QtWordEditor {

class GenericMathSpan;

/**
 * @brief 通用公式容器视图类（简化版）
 * 
 * 使用 TextBlockLayoutEngine，类似 TextBlockItem 的设计，
 * 但配置为 NoWrap 模式，没有段落、缩进等复杂功能。
 * 专门用于公式内部的文本和公式混排。
 */
class GenericMathItem : public MathItem
{
public:
    /**
     * @brief QGraphicsItem 类型标识
     */
    enum { Type = UserType + 2010 };

    /**
     * @brief 构造函数
     * @param span 对应的 GenericMathSpan 数据对象
     * @param parent 父 MathItem（可选）
     */
    GenericMathItem(GenericMathSpan *span, MathItem *parent = nullptr);

    /**
     * @brief 析构函数
     */
    ~GenericMathItem() override;

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
     * @brief 获取 QGraphicsItem 类型
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

    // ========== GenericMathItem 特有接口 ==========

    /**
     * @brief 判断是否是容器类型
     * @return 固定返回 true
     */
    bool isContainer() const override { return true; }
    
    /**
     * @brief 判断一个点在公式的哪个子 MathItem 区域
     * @param localPos 相对于自身的局部坐标
     * @return 子 MathItem 的索引，如果不在任何子 MathItem 区域返回 -1
     */
    int hitTestRegion(const QPointF &localPos) const override;

    /**
     * @brief 获取对应的 GenericMathSpan
     * @return GenericMathSpan 指针
     */
    GenericMathSpan *genericSpan() const;

    /**
     * @brief 光标视觉位置信息
     */
    struct CursorVisualInfo {
        QPointF position;  ///< 视觉位置（相对于 GenericMathItem）
        qreal height;      ///< 光标高度
    };

    /**
     * @brief 根据全局字符偏移获取视觉位置
     * @param globalOffset 全局字符偏移（从开头算起）
     * @return 视觉位置信息
     */
    CursorVisualInfo cursorPositionAt(int globalOffset) const;

    /**
     * @brief 根据相对于 GenericMathItem 的局部坐标获取全局字符偏移
     * @param localPos 局部坐标
     * @return 全局字符偏移（从开头算起）
     */
    int hitTest(const QPointF &localPos) const;

private:
    /**
     * @brief 更新 MathItem 子项
     */
    void updateMathItems();

    /**
     * @brief 清除 MathItem 子项
     */
    void clearMathItems();

    /**
     * @brief 获取 InlineSpan 列表
     */
    QList<InlineSpan*> getSpans() const;

    /**
     * @brief 执行布局
     */
    void performLayout();

    /**
     * @brief 执行布局（使用给定的 MathSpan 尺寸）
     */
    void performLayoutWithMathSizes(const QHash<InlineSpan*, QSizeF> &mathSizeMap, 
                                     const QHash<InlineSpan*, qreal> &mathBaselineMap);

    TextBlockLayoutEngine *m_layoutEngine;  ///< 布局引擎
    QList<MathItem*> m_mathItems;           ///< MathItem 子项列表
    QRectF m_boundingRect;                   ///< 边界矩形
    bool m_isUpdatingLayout;                 ///< 防止无限递归更新布局的标志
};

} // namespace QtWordEditor

#endif // GENERICMATHITEM_H
