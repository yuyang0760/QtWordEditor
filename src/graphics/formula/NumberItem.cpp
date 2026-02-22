#include "graphics/formula/NumberItem.h"
#include <QPainter>
#include <QFontMetricsF>

namespace QtWordEditor {

NumberItem::NumberItem(NumberMathSpan *dataSpan, QGraphicsItem *parent)
    : MathFormulaItem(dataSpan, parent)
    , m_font("Times New Roman", 20)
    , m_textColor(Qt::black)
{
    if (dataSpan) {
        m_text = dataSpan->text();
    }
    relayout();
}

void NumberItem::relayout()
{
    if (m_data) {
        NumberMathSpan *numSpan = static_cast<NumberMathSpan*>(m_data);
        m_text = numSpan->text();
    }
    calculateSize();
    updateBoundingRect();
    update();
}

void NumberItem::bindData(MathSpan *data)
{
    if (data && data->mathType() == MathSpan::Number) {
        m_data = data;
        NumberMathSpan *numSpan = static_cast<NumberMathSpan*>(m_data);
        m_text = numSpan->text();
        relayout();
    }
}

void NumberItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    
    painter->save();
    
    // 设置字体和颜色
    painter->setFont(m_font);
    painter->setPen(m_textColor);
    
    // 绘制文本（从基线位置开始）
    painter->drawText(QPointF(0, m_baseline), m_text);
    
    painter->restore();
}

void NumberItem::setFont(const QFont &font)
{
    m_font = font;
    relayout();
}

void NumberItem::setTextColor(const QColor &color)
{
    m_textColor = color;
    update();
}

void NumberItem::calculateSize()
{
    QFontMetricsF fm(m_font);
    m_width = fm.horizontalAdvance(m_text);
    m_height = fm.height();
    m_baseline = fm.ascent();
}

void NumberItem::updateBoundingRect()
{
    m_boundingRect = QRectF(0, 0, m_width, m_height);
}

QPointF NumberItem::cursorPosition(int charOffset) const
{
    // 使用 QFontMetricsF 计算指定偏移处的水平位置
    QFontMetricsF fm(m_font);
    QString textToOffset = m_text.left(qBound(0, charOffset, m_text.length()));
    qreal x = fm.horizontalAdvance(textToOffset);
    
    // y 坐标是基线位置
    return QPointF(x, m_baseline);
}

qreal NumberItem::cursorHeight() const
{
    // 光标高度是字体高度
    QFontMetricsF fm(m_font);
    return fm.height();
}

} // namespace QtWordEditor
