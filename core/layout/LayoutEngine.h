#ifndef LAYOUTENGINE_H
#define LAYOUTENGINE_H

#include <QObject>
#include <QHash>
#include "core/Global.h"

namespace QtWordEditor {

class Document;
class Block;

/**
 * @brief The LayoutEngine class is responsible for paginating document content
 * and computing positions and sizes of blocks.
 */
class LayoutEngine : public QObject
{
    Q_OBJECT
public:
    explicit LayoutEngine(QObject *parent = nullptr);
    ~LayoutEngine() override;

    void setDocument(Document *document);
    Document *document() const;

    // Page settings
    void setPageSize(qreal width, qreal height, qreal margin);
    void getPageSize(qreal *width, qreal *height, qreal *margin) const;

    // Perform full layout
    void layout();

    // Incremental layout from a given block index
    void layoutFrom(int blockIndex);

    // Calculate block height
    qreal calculateBlockHeight(Block *block, qreal maxWidth);

signals:
    void layoutChanged();

private:
    Document *m_document = nullptr;
    qreal m_pageWidth = 595.0;   // A4 width in points
    qreal m_pageHeight = 842.0;  // A4 height in points
    qreal m_margin = 72.0;       // 1 inch margin in points
    QHash<Block*, qreal> m_cachedHeights;
};

} // namespace QtWordEditor

#endif // LAYOUTENGINE_H