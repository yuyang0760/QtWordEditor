#include "graphics/items/BaseBlockItem.h"
#include "core/document/Block.h"
#include <QDebug>

namespace QtWordEditor {

BaseBlockItem::BaseBlockItem(Block *block, QGraphicsItem *parent)
    : QGraphicsRectItem(parent)
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

} // namespace QtWordEditor