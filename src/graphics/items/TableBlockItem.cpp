#include "graphics/items/TableBlockItem.h"
#include "core/document/TableBlock.h"
#include <QDebug>
#include <QPainter>

namespace QtWordEditor {

TableBlockItem::TableBlockItem(TableBlock *block, QGraphicsItem *parent)
    : BaseBlockItem(block, parent)
{
}

TableBlockItem::~TableBlockItem()
{
}

void TableBlockItem::updateBlock()
{
    // TODO: draw table grid and cell contents
}

QRectF TableBlockItem::boundingRect() const
{
    return m_boundingRect;
}

void TableBlockItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(painter);
    Q_UNUSED(option);
    Q_UNUSED(widget);
}

} // namespace QtWordEditor