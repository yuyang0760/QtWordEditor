#include "PageBuilder.h"
#include "core/document/Block.h"
#include "core/document/Page.h"
#include <QDebug>

namespace QtWordEditor {

PageBuilder::PageBuilder(qreal pageWidth, qreal pageHeight, qreal margin)
    : m_pageWidth(pageWidth)
    , m_pageHeight(pageHeight)
    , m_margin(margin)
    , m_contentWidth(pageWidth - 2 * margin)
    , m_contentHeight(pageHeight - 2 * margin)
    , m_currentY(0)
{
}

PageBuilder::~PageBuilder()
{
}

bool PageBuilder::tryAddBlock(Block *block)
{
    if (!block)
        return false;
    // Simplified: always add block, assume it fits
    m_currentPageBlocks.append(block);
    // Update current Y (placeholder)
    m_currentY += block->height();
    return true;
}

Page *PageBuilder::finishPage()
{
    // Create a page with default rect
    QRectF pageRect(0, 0, m_pageWidth, m_pageHeight);
    QRectF contentRect(m_margin, m_margin, m_contentWidth, m_contentHeight);
    Page *page = new Page(0, pageRect, contentRect);
    for (Block *block : m_currentPageBlocks) {
        page->addBlock(block);
    }
    m_currentPageBlocks.clear();
    m_currentY = 0;
    return page;
}

void PageBuilder::reset()
{
    m_currentPageBlocks.clear();
    m_currentY = 0;
}

} // namespace QtWordEditor