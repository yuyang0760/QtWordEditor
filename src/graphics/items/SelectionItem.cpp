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
    QBrush brush(QColor(0, 120, 215, 80)); // 恢复合适的透明度，背景色在下面，文字在上面
    painter->setBrush(brush);
    painter->setPen(Qt::NoPen); // 不绘制边框，避免相邻矩形重叠
    for (const QRectF &rect : m_rects) {
        painter->fillRect(rect, brush);
    }
    painter->restore();
}

} // namespace QtWordEditor
