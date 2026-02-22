#ifndef MATHFORMULAITEM_H
#define MATHFORMULAITEM_H

#include "core/document/MathSpan.h"
#include <QGraphicsItem>
#include <QRectF>
#include <QPainter>
#include "core/Global.h"

namespace QtWordEditor {

/**
 * @brief 公式图形项基类
 * 
 * 所有公式图形项都继承自此类，采用Composite设计模式。
 * 负责布局、测量、绘制，与渲染器对接。
 */
class MathFormulaItem : public QGraphicsItem
{
public:
    enum { Type = QGraphicsItem::UserType + 1020 };

    MathFormulaItem(MathSpan *dataSpan, QGraphicsItem *parent = nullptr);
    virtual ~MathFormulaItem() = default;

    int type() const override { return Type; }
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    /**
     * @brief 重新计算布局
     */
    virtual void relayout() = 0;

    /**
     * @brief 绑定数据
     */
    virtual void bindData(MathSpan *data) = 0;

    /**
     * @brief 获取数据
     */
    MathSpan *data() const { return m_data; }

    // ========== 排版尺寸 ==========
    /**
     * @brief 宽度
     */
    qreal width() const { return m_width; }
    
    /**
     * @brief 高度
     */
    qreal height() const { return m_height; }
    
    /**
     * @brief 基线高度
     */
    qreal baseline() const { return m_baseline; }

    // ========== 光标定位 ==========
    
    /**
     * @brief 获取指定子项索引处的光标位置
     * @param childIndex 子项索引（0 <= childIndex <= childCount()）
     * @return 光标位置（局部坐标）
     */
    virtual QPointF cursorPosition(int childIndex) const;
    
    /**
     * @brief 获取光标高度
     * @return 光标高度
     */
    virtual qreal cursorHeight() const;
    
    /**
     * @brief 获取子项数量
     * @return 子项数量
     */
    virtual int childCount() const { return 0; }
    
    /**
     * @brief 获取指定索引的子项
     * @param index 子项索引
     * @return 子项指针
     */
    virtual MathFormulaItem *childAt(int index) const { Q_UNUSED(index); return nullptr; }

protected:
    MathSpan *m_data;
    qreal m_width;
    qreal m_height;
    qreal m_baseline;
    QRectF m_boundingRect;
};

} // namespace QtWordEditor

#endif // MATHFORMULAITEM_H
