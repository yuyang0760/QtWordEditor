#ifndef DOCUMENTSCENE_H
#define DOCUMENTSCENE_H

#include <QGraphicsScene>
#include <QHash>
#include <QList>
#include "core/Global.h"

namespace QtWordEditor {

class Document;
class Block;
class Page;
class BaseBlockItem;
class CursorItem;
class SelectionItem;
class PageItem;
struct CursorPosition;

/**
 * @brief The DocumentScene class manages the graphical representation of a document.
 */
class DocumentScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit DocumentScene(QObject *parent = nullptr);
    ~DocumentScene() override;

    void setDocument(Document *document);
    Document *document() const;

    // Rebuild the entire scene from the document.
    void rebuildFromDocument();

    // Update all text items content without rebuilding.
    void updateAllTextItems();

    // Update cursor position (world coordinates).
    void updateCursor(const QPointF &pos, qreal height);

    // Update selection rectangles.
    void updateSelection(const QList<QRectF> &rects);
    void clearSelection();

    // Page management
    void clearPages();
    void addPage(Page *page);
    Page *pageAt(const QPointF &scenePos) const;
    
    // Cursor position from scene position
    CursorPosition cursorPositionAt(const QPointF &scenePos) const;

public slots:
    void onBlockAdded(int globalIndex);
    void onBlockRemoved(int globalIndex);
    void onLayoutChanged();

private:
    Document *m_document;
    QHash<Block*, BaseBlockItem*> m_blockItems;
    QList<PageItem*> m_pageItems;
    CursorItem *m_cursorItem;
    SelectionItem *m_selectionItem;
};

} // namespace QtWordEditor

#endif // DOCUMENTSCENE_H