#include "graphics/formula/FractionItem.h"
#include "graphics/factory/MathFormulaItemFactory.h"
#include <QPainter>

namespace QtWordEditor {

FractionItem::FractionItem(FractionMathSpan *dataSpan, QGraphicsItem *parent)
    : MathFormulaItem(dataSpan, parent)
    , m_numeratorItem(nullptr)
    , m_denominatorItem(nullptr)
    , m_lineThickness(1.5)
    , m_gap(3)
{
    createChildItems();
    relayout();
}

void FractionItem::relayout()
{
    createChildItems();
    layoutNumeratorAndDenominator();
    calculateSize();
    updateBoundingRect();
    update();
}

void FractionItem::bindData(MathSpan *data)
{
    if (data && data->mathType() == MathSpan::Fraction) {
        m_data = data;
        relayout();
    }
}

void FractionItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    
    painter->save();
    
    // 绘制分数线
    QPen pen(Qt::black, m_lineThickness);
    pen.setCapStyle(Qt::FlatCap);
    painter->setPen(pen);
    
    qreal lineY = m_numeratorItem ? m_numeratorItem->height() + m_gap : 0;
    painter->drawLine(QPointF(0, lineY), QPointF(m_width, lineY));
    
    painter->restore();
}

void FractionItem::layoutNumeratorAndDenominator()
{
    qreal maxWidth = 0;
    
    // 计算最大宽度
    if (m_numeratorItem) {
        maxWidth = qMax(maxWidth, m_numeratorItem->width());
    }
    if (m_denominatorItem) {
        maxWidth = qMax(maxWidth, m_denominatorItem->width());
    }
    
    // 布局分子
    if (m_numeratorItem) {
        qreal numX = (maxWidth - m_numeratorItem->width()) / 2.0;
        m_numeratorItem->setPos(numX, 0);
    }
    
    // 布局分母
    qreal lineY = m_numeratorItem ? m_numeratorItem->height() + m_gap : 0;
    if (m_denominatorItem) {
        qreal denX = (maxWidth - m_denominatorItem->width()) / 2.0;
        qreal denY = lineY + m_lineThickness + m_gap;
        m_denominatorItem->setPos(denX, denY);
    }
}

void FractionItem::clearChildren()
{
    delete m_numeratorItem;
    m_numeratorItem = nullptr;
    
    delete m_denominatorItem;
    m_denominatorItem = nullptr;
}

void FractionItem::createChildItems()
{
    clearChildren();
    
    if (!m_data) {
        return;
    }
    
    FractionMathSpan *fracSpan = static_cast<FractionMathSpan*>(m_data);
    
    // 创建分子项
    if (fracSpan->numerator()) {
        m_numeratorItem = MathFormulaItemFactory::createItem(fracSpan->numerator(), this);
    }
    
    // 创建分母项
    if (fracSpan->denominator()) {
        m_denominatorItem = MathFormulaItemFactory::createItem(fracSpan->denominator(), this);
    }
}

void FractionItem::calculateSize()
{
    qreal maxWidth = 0;
    qreal totalHeight = 0;
    
    if (m_numeratorItem) {
        maxWidth = qMax(maxWidth, m_numeratorItem->width());
        totalHeight += m_numeratorItem->height();
        totalHeight += m_gap;
    }
    
    totalHeight += m_lineThickness;
    totalHeight += m_gap;
    
    if (m_denominatorItem) {
        maxWidth = qMax(maxWidth, m_denominatorItem->width());
        totalHeight += m_denominatorItem->height();
    }
    
    m_width = maxWidth;
    m_height = totalHeight;
    
    // 基线在分子底部 + 半个间距
    if (m_numeratorItem) {
        m_baseline = m_numeratorItem->baseline();
    } else {
        m_baseline = m_height / 2.0;
    }
}

void FractionItem::updateBoundingRect()
{
    m_boundingRect = QRectF(0, 0, m_width, m_height);
}

} // namespace QtWordEditor
