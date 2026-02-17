#include "CursorItem.h"
#include <QObject>
#include <QPen>
#include <QBrush>
#include <QDebug>

namespace QtWordEditor {

CursorItem::CursorItem(QGraphicsItem *parent)
    : QGraphicsLineItem(parent)
    , m_visible(true)
{
    setPen(QPen(Qt::black, 1));
    m_blinkTimer.setInterval(500);
    QObject::connect(&m_blinkTimer, &QTimer::timeout, this, &CursorItem::toggleBlink);
    startBlink();
}

CursorItem::~CursorItem()
{
}

void CursorItem::setPosition(const QPointF &pos, qreal height)
{
    setLine(pos.x(), pos.y(), pos.x(), pos.y() + height);
}

void CursorItem::startBlink()
{
    m_blinkTimer.start();
}

void CursorItem::stopBlink()
{
    m_blinkTimer.stop();
    setVisible(true);
}

void CursorItem::toggleBlink()
{
    m_visible = !m_visible;
    setVisible(m_visible);
}

} // namespace QtWordEditor