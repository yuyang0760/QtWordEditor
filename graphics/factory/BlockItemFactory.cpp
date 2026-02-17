#include "BlockItemFactory.h"
#include "core/document/Block.h"
#include "core/document/ParagraphBlock.h"
#include "core/document/ImageBlock.h"
#include "core/document/TableBlock.h"
#include "graphics/items/TextBlockItem.h"
#include "graphics/items/ImageBlockItem.h"
#include "graphics/items/TableBlockItem.h"
#include <QDebug>

namespace QtWordEditor {

BaseBlockItem *BlockItemFactory::createItem(Block *block, QGraphicsItem *parent)
{
    if (!block)
        return nullptr;

    ParagraphBlock *para = qobject_cast<ParagraphBlock*>(block);
    if (para) {
        return new TextBlockItem(para, parent);
    }

    ImageBlock *img = qobject_cast<ImageBlock*>(block);
    if (img) {
        return new ImageBlockItem(img, parent);
    }

    TableBlock *tab = qobject_cast<TableBlock*>(block);
    if (tab) {
        return new TableBlockItem(tab, parent);
    }

    qWarning() << "Unknown block type";
    return nullptr;
}

} // namespace QtWordEditor