#include "graphics/items/BaseBlockItem.h"
#include "core/document/Block.h"
#include <QPainter>
#include <QDebug>

namespace QtWordEditor {

BaseBlockItem::BaseBlockItem(Block *block, QGraphicsItem *parent)
    : QGraphicsItem(parent)
    , m_block(block)
{
}

BaseBlockItem::~BaseBlockItem()
{
}

Block *BaseBlockItem::block() const
{
    return m_block;
}

QRectF BaseBlockItem::boundingRect() const
{
    return m_boundingRect;
}

void BaseBlockItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(painter);
    Q_UNUSED(option);
    Q_UNUSED(widget);
}

void BaseBlockItem::setRect(const QRectF &rect)
{
    if (m_boundingRect != rect) {
        prepareGeometryChange();
        m_boundingRect = rect;
    }
}

void BaseBlockItem::setRect(qreal x, qreal y, qreal w, qreal h)
{
    setRect(QRectF(x, y, w, h));
}

QRectF BaseBlockItem::rect() const
{
    return m_boundingRect;
}

} // namespace QtWordEditor