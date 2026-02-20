#ifndef TABLEBLOCKITEM_H
#define TABLEBLOCKITEM_H

#include "BaseBlockItem.h"
#include "core/Global.h"

namespace QtWordEditor {

class TableBlock;

/**
 * @brief The TableBlockItem class renders a table block.
 */
class TableBlockItem : public BaseBlockItem
{
public:
    explicit TableBlockItem(TableBlock *block, QGraphicsItem *parent = nullptr);
    ~TableBlockItem() override;

    void updateBlock() override;
    
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    // TODO: implement table rendering
};

} // namespace QtWordEditor

#endif // TABLEBLOCKITEM_H