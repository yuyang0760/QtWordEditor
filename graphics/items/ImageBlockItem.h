#ifndef IMAGEBLOCKITEM_H
#define IMAGEBLOCKITEM_H

#include "BaseBlockItem.h"
#include <QGraphicsPixmapItem>
#include "core/Global.h"

namespace QtWordEditor {

class ImageBlock;

/**
 * @brief The ImageBlockItem class renders an image block.
 */
class ImageBlockItem : public BaseBlockItem
{
public:
    explicit ImageBlockItem(ImageBlock *block, QGraphicsItem *parent = nullptr);
    ~ImageBlockItem() override;

    void updateBlock() override;

private:
    QGraphicsPixmapItem *m_pixmapItem;
};

} // namespace QtWordEditor

#endif // IMAGEBLOCKITEM_H