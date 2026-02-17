#ifndef IMAGEBLOCK_H
#define IMAGEBLOCK_H

#include "Block.h"
#include <QImage>
#include <QSizeF>
#include "core/Global.h"

namespace QtWordEditor {

/**
 * @brief The ImageBlock class represents an image block.
 */
class ImageBlock : public Block
{
    Q_OBJECT
public:
    explicit ImageBlock(QObject *parent = nullptr);
    ImageBlock(const ImageBlock &other);
    ~ImageBlock() override;

    // Image data
    QImage image() const;
    void setImage(const QImage &image);

    // Display size
    QSizeF size() const;
    void setSize(const QSizeF &size);

    // Caption
    QString caption() const;
    void setCaption(const QString &caption);

    // Overrides from Block
    int length() const override;
    bool isEmpty() const override;
    Block *clone() const override;

private:
    QImage m_image;
    QSizeF m_size;
    QString m_caption;
};

} // namespace QtWordEditor

#endif // IMAGEBLOCK_H