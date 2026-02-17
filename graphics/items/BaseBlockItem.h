#ifndef BASEBLOCKITEM_H
#define BASEBLOCKITEM_H

#include <QGraphicsRectItem>
#include "core/Global.h"

namespace QtWordEditor {

class Block;

/**
 * @brief The BaseBlockItem class is the base class for all block graphics items.
 */
class BaseBlockItem : public QGraphicsRectItem
{
public:
    explicit BaseBlockItem(Block *block, QGraphicsItem *parent = nullptr);
    ~BaseBlockItem() override;

    Block *block() const;
    virtual void updateBlock() = 0;

protected:
    Block *m_block;
};

} // namespace QtWordEditor

#endif // BASEBLOCKITEM_H