#ifndef BLOCKITEMFACTORY_H
#define BLOCKITEMFACTORY_H

#include "core/Global.h"

class QGraphicsItem;

namespace QtWordEditor {

class Block;
class BaseBlockItem;

/**
 * @brief The BlockItemFactory class creates appropriate graphics items for blocks.
 */
class BlockItemFactory
{
public:
    static BaseBlockItem *createItem(Block *block, QGraphicsItem *parent = nullptr);
};

} // namespace QtWordEditor

#endif // BLOCKITEMFACTORY_H