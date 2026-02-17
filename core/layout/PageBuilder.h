#ifndef PAGEBUILDER_H
#define PAGEBUILDER_H

#include <QList>
#include "core/Global.h"

namespace QtWordEditor {

class Block;
class Page;

/**
 * @brief The PageBuilder class assists the layout engine in distributing blocks across pages.
 */
class PageBuilder
{
public:
    PageBuilder(qreal pageWidth, qreal pageHeight, qreal margin);
    ~PageBuilder();

    // Try to add a block to the current page; returns true if successful.
    bool tryAddBlock(Block *block);

    // Finish the current page and return a Page object.
    Page *finishPage();

    // Reset for a new page.
    void reset();

private:
    qreal m_pageWidth;
    qreal m_pageHeight;
    qreal m_margin;
    qreal m_contentWidth;
    qreal m_contentHeight;
    qreal m_currentY;
    QList<Block*> m_currentPageBlocks;
};

} // namespace QtWordEditor

#endif // PAGEBUILDER_H