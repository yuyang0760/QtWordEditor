#include "core/document/Section.h"
#include "core/document/Block.h"
#include "core/document/Page.h"
#include <QDebug>

namespace QtWordEditor {

Section::Section(QObject *parent)
    : QObject(parent)
{
}

Section::~Section()
{
    qDeleteAll(m_blocks);
    qDeleteAll(m_pages);
}

int Section::sectionNumber() const
{
    return m_sectionNumber;
}

void Section::setSectionNumber(int number)
{
    if (m_sectionNumber != number) {
        m_sectionNumber = number;
        // Notify if needed
    }
}

QString Section::header() const
{
    return m_header;
}

void Section::setHeader(const QString &header)
{
    if (m_header != header) {
        m_header = header;
        // Notify if needed
    }
}

QString Section::footer() const
{
    return m_footer;
}

void Section::setFooter(const QString &footer)
{
    if (m_footer != footer) {
        m_footer = footer;
        // Notify if needed
    }
}

int Section::blockCount() const
{
    return m_blocks.size();
}

Block *Section::block(int index) const
{
    if (index >= 0 && index < m_blocks.size())
        return m_blocks.at(index);
    return nullptr;
}

void Section::addBlock(Block *block)
{
    insertBlock(m_blocks.size(), block);
}

void Section::insertBlock(int index, Block *block)
{
    if (!block || index < 0 || index > m_blocks.size())
        return;
    block->setParent(this);
    m_blocks.insert(index, block);
    emit blockAdded(index);
}

void Section::removeBlock(int index)
{
    if (index < 0 || index >= m_blocks.size())
        return;
    Block *block = m_blocks.takeAt(index);
    block->deleteLater();
    emit blockRemoved(index);
}

int Section::pageCount() const
{
    return m_pages.size();
}

Page *Section::page(int index) const
{
    if (index >= 0 && index < m_pages.size())
        return m_pages.at(index);
    return nullptr;
}

void Section::addPage(Page *page)
{
    if (!page)
        return;
    m_pages.append(page);
    emit pagesChanged();
}

void Section::clearPages()
{
    qDeleteAll(m_pages);
    m_pages.clear();
    emit pagesChanged();
}

} // namespace QtWordEditor