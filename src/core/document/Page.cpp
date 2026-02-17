#include "include/core/document/Page.h"
#include "include/core/document/Block.h"
#include <QDebug>

namespace QtWordEditor {

Page::Page(int pageNumber, const QRectF &pageRect, const QRectF &contentRect)
    : m_pageNumber(pageNumber)
    , m_pageRect(pageRect)
    , m_contentRect(contentRect)
{
}

Page::~Page()
{
}

int Page::pageNumber() const
{
    return m_pageNumber;
}

QRectF Page::pageRect() const
{
    return m_pageRect;
}

QRectF Page::contentRect() const
{
    return m_contentRect;
}

int Page::blockCount() const
{
    return m_blocks.size();
}

Block *Page::block(int index) const
{
    if (index >= 0 && index < m_blocks.size())
        return m_blocks.at(index);
    return nullptr;
}

void Page::addBlock(Block *block)
{
    if (block) {
        m_blocks.append(block);
    }
}

void Page::clearBlocks()
{
    m_blocks.clear();
}

bool Page::isEmpty() const
{
    return m_blocks.isEmpty();
}

} // namespace QtWordEditor