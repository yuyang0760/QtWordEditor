#include "graphics/items/BaseBlockItem.h"
#include "core/document/Block.h"
#include <QDebug>

namespace QtWordEditor {

BaseBlockItem::BaseBlockItem(Block *block, QGraphicsItem *parent)
    : QGraphicsItem(parent)
    , m_block(block)
    , m_boundingRect(0, 0, 0, 0)
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