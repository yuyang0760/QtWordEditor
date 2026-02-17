#ifndef BLOCK_H
#define BLOCK_H

#include <QObject>
#include <QRectF>
#include "core/Global.h"

namespace QtWordEditor {

/**
 * @brief The Block class is the abstract base class for all content blocks.
 */
class Block : public QObject
{
    Q_OBJECT
public:
    explicit Block(QObject *parent = nullptr);
    ~Block() override;

    // Identification
    int blockId() const;
    void setBlockId(int id);

    // Geometry (set by layout engine)
    QRectF boundingRect() const;
    void setBoundingRect(const QRectF &rect);

    qreal height() const;
    void setHeight(qreal height);

    // Position in document (global index, managed by Document)
    int positionInDocument() const;
    void setPositionInDocument(int pos);

    // Abstract methods
    virtual int length() const = 0;
    virtual bool isEmpty() const = 0;
    virtual Block *clone() const = 0;

signals:
    void geometryChanged();

private:
    int m_blockId = -1;
    QRectF m_boundingRect;
    qreal m_height = 0.0;
    int m_positionInDocument = -1;
};

} // namespace QtWordEditor

#endif // BLOCK_H