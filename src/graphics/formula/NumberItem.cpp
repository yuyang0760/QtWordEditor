/**
 * @file NumberItem.cpp
 * @brief 数字/变量视图类实现
 */

#include "graphics/formula/NumberItem.h"
#include "core/document/math/NumberMathSpan.h"
#include <QFontMetrics>
#include <QPainter>

namespace QtWordEditor {

NumberItem::NumberItem(NumberMathSpan *span, MathItem *parent)
    : MathItem(span, parent)
    , m_font("Cambria Math", 12)
{
    qDebug() << "[NumberItem::NumberItem] 构造函数开始, span=" << span;
    updateLayout();
    qDebug() << "[NumberItem::NumberItem] 构造函数完成";
}

NumberItem::~NumberItem()
{
}

void NumberItem::updateLayout()
{
    qDebug() << "[NumberItem::updateLayout] 开始...";
    NumberMathSpan *numSpan = numberSpan();
    if (!numSpan) {
        qDebug() << "[NumberItem::updateLayout] numSpan 为空，返回";
        return;
    }
    qDebug() << "[NumberItem::updateLayout] numSpan 有效, text=[" << numSpan->text() << "]";

    QFontMetrics fm(m_font);
    QString text = numSpan->text();
    
    if (text.isEmpty()) {
        text = " ";  // 空时显示一个占位符
    }
    qDebug() << "[NumberItem::updateLayout] 显示的文本: [" << text << "]";

    qreal width = fm.horizontalAdvance(text);
    qreal height = fm.height();
    qreal ascent = fm.ascent();
    qDebug() << "[NumberItem::updateLayout] width=" << width << ", height=" << height << ", ascent=" << ascent;

    m_boundingRect = QRectF(0, 0, width, height);
    m_baseline = ascent;
    qDebug() << "[NumberItem::updateLayout] m_boundingRect=" << m_boundingRect << ", m_baseline=" << m_baseline;

    qDebug() << "[NumberItem::updateLayout] 完成";
}

qreal NumberItem::baseline() const
{
    return m_baseline;
}

QRectF NumberItem::boundingRect() const
{
    return m_boundingRect;
}

void NumberItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    NumberMathSpan *numSpan = numberSpan();
    if (!numSpan) return;

    painter->save();
    painter->setFont(m_font);
    painter->setPen(Qt::black);

    QString text = numSpan->text();
    if (text.isEmpty()) {
        text = " ";
    }

    painter->drawText(QPointF(0, m_baseline), text);

    painter->restore();
}

NumberMathSpan *NumberItem::numberSpan() const
{
    return static_cast<NumberMathSpan*>(m_span);
}

} // namespace QtWordEditor
