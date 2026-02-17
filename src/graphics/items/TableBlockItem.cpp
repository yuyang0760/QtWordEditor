#include "graphics/items/TableBlockItem.h"
#include "core/document/TableBlock.h"
#include <QDebug>

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

} // namespace QtWordEditor