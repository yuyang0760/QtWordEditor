#include "DocumentScene.h"
#include "core/document/Document.h"
#include "core/document/Block.h"
#include "graphics/items/BaseBlockItem.h"
#include "graphics/items/CursorItem.h"
#include "graphics/items/SelectionItem.h"
#include <QDebug>

namespace QtWordEditor {

DocumentScene::DocumentScene(QObject *parent)
    : QGraphicsScene(parent)
    , m_document(nullptr)
    , m_cursorItem(nullptr)
    , m_selectionItem(nullptr)
{
}

DocumentScene::~DocumentScene()
{
}

void DocumentScene::setDocument(Document *document)
{
    if (m_document == document)
        return;
    m_document = document;
    rebuildFromDocument();
}

Document *DocumentScene::document() const
{
    return m_document;
}

void DocumentScene::rebuildFromDocument()
{
    clear();
    m_blockItems.clear();
    // TODO: implement actual rebuilding
    qDebug() << "DocumentScene::rebuildFromDocument() placeholder";
}

void DocumentScene::updateCursor(const QPointF &pos, qreal height)
{
    if (!m_cursorItem) {
        m_cursorItem = new CursorItem();
        addItem(m_cursorItem);
    }
    m_cursorItem->setPosition(pos, height);
}

void DocumentScene::updateSelection(const QList<QRectF> &rects)
{
    if (!m_selectionItem) {
        m_selectionItem = new SelectionItem();
        addItem(m_selectionItem);
    }
    m_selectionItem->setRects(rects);
}

void DocumentScene::clearSelection()
{
    if (m_selectionItem) {
        m_selectionItem->clear();
    }
}

void DocumentScene::onBlockAdded(int globalIndex)
{
    Q_UNUSED(globalIndex);
    // TODO: incremental update
}

void DocumentScene::onBlockRemoved(int globalIndex)
{
    Q_UNUSED(globalIndex);
    // TODO: incremental update
}

void DocumentScene::onLayoutChanged()
{
    // TODO: update block positions
}

} // namespace QtWordEditor