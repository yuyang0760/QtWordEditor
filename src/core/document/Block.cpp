#include "core/document/Block.h"
#include <QDebug>

namespace QtWordEditor {

Block::Block(QObject *parent)
    : QObject(parent)
{
}

Block::~Block()
{
}

int Block::blockId() const
{
    return m_blockId;
}

void Block::setBlockId(int id)
{
    if (m_blockId != id) {
        m_blockId = id;
    }
}

QRectF Block::boundingRect() const
{
    return m_boundingRect;
}

void Block::setBoundingRect(const QRectF &rect)
{
    if (m_boundingRect != rect) {
        m_boundingRect = rect;
        emit geometryChanged();
    }
}

qreal Block::height() const
{
    return m_height;
}

void Block::setHeight(qreal height)
{
    if (!qFuzzyCompare(m_height, height)) {
        m_height = height;
        emit geometryChanged();
    }
}

int Block::positionInDocument() const
{
    return m_positionInDocument;
}

void Block::setPositionInDocument(int pos)
{
    if (m_positionInDocument != pos) {
        m_positionInDocument = pos;
    }
}

} // namespace QtWordEditor