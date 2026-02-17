#include "graphics/items/ImageBlockItem.h"
#include "core/document/ImageBlock.h"
#include <QDebug>

namespace QtWordEditor {

ImageBlockItem::ImageBlockItem(ImageBlock *block, QGraphicsItem *parent)
    : BaseBlockItem(block, parent)
    , m_pixmapItem(new QGraphicsPixmapItem(this))
{
}

ImageBlockItem::~ImageBlockItem()
{
}

void ImageBlockItem::updateBlock()
{
    ImageBlock *img = qobject_cast<ImageBlock*>(m_block);
    if (!img)
        return;

    QImage image = img->image();
    if (!image.isNull()) {
        m_pixmapItem->setPixmap(QPixmap::fromImage(image));
        // Scale to display size
        QSizeF size = img->size();
        if (size.isValid()) {
            m_pixmapItem->setScale(size.width() / image.width());
        }
        m_pixmapItem->setPos(m_block->boundingRect().topLeft());
    }
}

} // namespace QtWordEditor