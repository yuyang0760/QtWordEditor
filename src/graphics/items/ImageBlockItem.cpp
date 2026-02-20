#include "graphics/items/ImageBlockItem.h"
#include "core/document/ImageBlock.h"
#include <QDebug>
#include <QPainter>

namespace QtWordEditor {

ImageBlockItem::ImageBlockItem(ImageBlock *block, QGraphicsItem *parent)
    : BaseBlockItem(block, parent)
    , m_pixmapItem(new QGraphicsPixmapItem(this))
{
}

ImageBlockItem::~ImageBlockItem()
{
}

QRectF ImageBlockItem::boundingRect() const
{
    return m_boundingRect;
}

void ImageBlockItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(painter);
    Q_UNUSED(option);
    Q_UNUSED(widget);
    // m_pixmapItem 作为子项会自动绘制
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
        m_pixmapItem->setPos(0, 0);
        
        // 更新边界矩形
        m_boundingRect = QRectF(0, 0, size.width(), size.height());
    }
}

} // namespace QtWordEditor