#include "graphics/items/SelectionItem.h"
#include <QPainter>
#include <QDebug>

namespace QtWordEditor {

SelectionItem::SelectionItem(QGraphicsItem *parent)
    : QGraphicsItem(parent)
{
    setFlag(QGraphicsItem::ItemIsSelectable, false);
    setFlag(QGraphicsItem::ItemIsFocusable, false);
}

SelectionItem::~SelectionItem()
{
}

void SelectionItem::setRects(const QList<QRectF> &rects)
{
    prepareGeometryChange();
    m_rects = rects;
    update();
}

void SelectionItem::clear()
{
    prepareGeometryChange();
    m_rects.clear();
    update();
}

QRectF SelectionItem::boundingRect() const
{
    if (m_rects.isEmpty())
        return QRectF();
    QRectF bounds;
    for (const QRectF &rect : m_rects) {
        bounds = bounds.united(rect);
    }
    return bounds;
}

void SelectionItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                          QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    painter->save();
    QBrush brush(QColor(0, 120, 215, 80)); // semi‑transparent blue
    QPen pen(QColor(0, 90, 180, 160), 1);
    painter->setBrush(brush);
    painter->setPen(pen);
    for (const QRectF &rect : m_rects) {
        painter->drawRect(rect);
    }
    painter->restore();
}

} // namespace QtWordEditor