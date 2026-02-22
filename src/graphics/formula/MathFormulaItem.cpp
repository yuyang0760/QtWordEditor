#include "graphics/formula/MathFormulaItem.h"
#include <QPainter>

namespace QtWordEditor {

MathFormulaItem::MathFormulaItem(MathSpan *dataSpan, QGraphicsItem *parent)
    : QGraphicsItem(parent)
    , m_data(dataSpan)
    , m_width(0)
    , m_height(0)
    , m_baseline(0)
{
    m_boundingRect = QRectF(0, 0, m_width, m_height);
}

QRectF MathFormulaItem::boundingRect() const
{
    return m_boundingRect;
}

void MathFormulaItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    
    // 基类不做具体绘制，由子类实现
}

QPointF MathFormulaItem::cursorPosition(int childIndex) const
{
    Q_UNUSED(childIndex);
    // 默认返回左下角位置
    return QPointF(0, m_baseline);
}

qreal MathFormulaItem::cursorHeight() const
{
    // 默认返回高度
    return m_height;
}

} // namespace QtWordEditor
