#include "ImageBlock.h"
#include <QDebug>

namespace QtWordEditor {

ImageBlock::ImageBlock(QObject *parent)
    : Block(parent)
{
}

ImageBlock::ImageBlock(const ImageBlock &other)
    : Block(other.parent())
    , m_image(other.m_image)
    , m_size(other.m_size)
    , m_caption(other.m_caption)
{
}

ImageBlock::~ImageBlock()
{
}

QImage ImageBlock::image() const
{
    return m_image;
}

void ImageBlock::setImage(const QImage &image)
{
    m_image = image;
}

QSizeF ImageBlock::size() const
{
    return m_size;
}

void ImageBlock::setSize(const QSizeF &size)
{
    m_size = size;
}

QString ImageBlock::caption() const
{
    return m_caption;
}

void ImageBlock::setCaption(const QString &caption)
{
    m_caption = caption;
}

int ImageBlock::length() const
{
    return 1; // Non‑text blocks have length 1 for cursor navigation.
}

bool ImageBlock::isEmpty() const
{
    return m_image.isNull();
}

Block *ImageBlock::clone() const
{
    ImageBlock *copy = new ImageBlock(parent());
    copy->m_image = m_image;
    copy->m_size = m_size;
    copy->m_caption = m_caption;
    copy->setBlockId(blockId());
    copy->setBoundingRect(boundingRect());
    copy->setHeight(height());
    copy->setPositionInDocument(positionInDocument());
    return copy;
}

} // namespace QtWordEditor