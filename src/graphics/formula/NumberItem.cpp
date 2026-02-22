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
    , m_font("Microsoft YaHei", 12)
{
    updateLayout();
}

NumberItem::~NumberItem()
{
}

void NumberItem::updateLayout()
{
    NumberMathSpan *numSpan = numberSpan();
    if (!numSpan) {
        return;
    }

    QFontMetrics fm(m_font);
    QString text = numSpan->text();
    
    if (text.isEmpty()) {
        text = " ";  // 空时显示一个占位符
    }

    qreal width = fm.horizontalAdvance(text);
    qreal height = fm.height();
    qreal ascent = fm.ascent();

    m_boundingRect = QRectF(0, 0, width, height);
    m_baseline = ascent;
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
    
    // 绘制虚线边框
    QPen borderPen(QColor(128, 128, 128)); // 灰色
    borderPen.setStyle(Qt::DashLine);
    borderPen.setWidth(1);
    painter->setPen(borderPen);
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(m_boundingRect.adjusted(0.5, 0.5, -0.5, -0.5)); // 调整位置避免模糊
    
    // 绘制文本
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

qreal NumberItem::cursorXAt(int position) const
{
    NumberMathSpan *numSpan = numberSpan();
    if (!numSpan) {
        return 0;
    }

    QString text = numSpan->text();
    if (position <= 0) {
        return 0;
    }

    if (position >= text.length()) {
        return boundingRect().width();
    }

    // 计算到第 position 个字符的宽度
    QFontMetrics fm(m_font);
    QString subText = text.left(position);
    return fm.horizontalAdvance(subText);
}

int NumberItem::textLength() const
{
    NumberMathSpan *numSpan = numberSpan();
    if (!numSpan) {
        return 0;
    }
    return numSpan->text().length();
}

int NumberItem::hitTestX(qreal x) const
{
    NumberMathSpan *numSpan = numberSpan();
    if (!numSpan) {
        return 0;
    }

    QString text = numSpan->text();
    if (text.isEmpty()) {
        return 0;
    }

    QFontMetrics fm(m_font);
    // 从第一个字符开始，找到最合适的位置
    for (int i = 0; i <= text.length(); ++i) {
        QString subText = text.left(i);
        qreal charX = fm.horizontalAdvance(subText);
        if (x <= charX) {
            return i;
        }
    }
    return text.length();
}

} // namespace QtWordEditor
