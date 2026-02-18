
/**
 * @file Section.cpp
 * @brief Implementation of the Section class
 *
 * This file contains the implementation of the Section class which represents
 * a logical section within a document, containing blocks and pages.
 */

#include "core/document/Section.h"
#include "core/document/Block.h"
#include "core/document/Page.h"
#include &lt;QDebug&gt;

namespace QtWordEditor {

/**
 * @brief Constructs a Section object
 * @param parent Parent QObject
 */
Section::Section(QObject *parent)
    : QObject(parent)
{
}

/**
 * @brief Destroys the Section object and cleans up blocks and pages
 */
Section::~Section()
{
    qDeleteAll(m_blocks);
    qDeleteAll(m_pages);
}

/**
 * @brief Gets the section number
 * @return Section number
 */
int Section::sectionNumber() const
{
    return m_sectionNumber;
}

/**
 * @brief Sets the section number
 * @param number New section number
 */
void Section::setSectionNumber(int number)
{
    if (m_sectionNumber != number) {
        m_sectionNumber = number;
    }
}

/**
 * @brief Gets the section header text
 * @return Header text
 */
QString Section::header() const
{
    return m_header;
}

/**
 * @brief Sets the section header text
 * @param header New header text
 */
void Section::setHeader(const QString &amp;header)
{
    if (m_header != header) {
        m_header = header;
    }
}

/**
 * @brief Gets the section footer text
 * @return Footer text
 */
QString Section::footer() const
{
    return m_footer;
}

/**
 * @brief Sets the section footer text
 * @param footer New footer text
 */
void Section::setFooter(const QString &amp;footer)
{
    if (m_footer != footer) {
        m_footer = footer;
    }
}

/**
 * @brief Gets the number of blocks in this section
 * @return Block count
 */
int Section::blockCount() const
{
    return m_blocks.size();
}

/**
 * @brief Gets a block by index
 * @param index Block index
 * @return Pointer to Block, or nullptr if index is invalid
 */
Block *Section::block(int index) const
{
    if (index &gt;= 0 &amp;&amp; index &lt; m_blocks.size())
        return m_blocks.at(index);
    return nullptr;
}

/**
 * @brief Adds a block to the end of the section
 * @param block Block to add
 */
void Section::addBlock(Block *block)
{
    insertBlock(m_blocks.size(), block);
}

/**
 * @brief Inserts a block at the specified index
 * @param index Insert position
 * @param block Block to insert
 */
void Section::insertBlock(int index, Block *block)
{
    if (!block || index &lt; 0 || index &gt; m_blocks.size())
        return;
    block-&gt;setParent(this);
    m_blocks.insert(index, block);
    emit blockAdded(index);
}

/**
 * @brief Removes a block at the specified index
 * @param index Block index to remove
 */
void Section::removeBlock(int index)
{
    if (index &lt; 0 || index &gt;= m_blocks.size())
        return;
    Block *block = m_blocks.takeAt(index);
    block-&gt;deleteLater();
    emit blockRemoved(index);
}

/**
 * @brief Gets the number of pages in this section
 * @return Page count
 */
int Section::pageCount() const
{
    return m_pages.size();
}

/**
 * @brief Gets a page by index
 * @param index Page index
 * @return Pointer to Page, or nullptr if index is invalid
 */
Page *Section::page(int index) const
{
    if (index &gt;= 0 &amp;&amp; index &lt; m_pages.size())
        return m_pages.at(index);
    return nullptr;
}

/**
 * @brief Adds a page to the section
 * @param page Page to add
 */
void Section::addPage(Page *page)
{
    if (!page)
        return;
    m_pages.append(page);
    emit pagesChanged();
}

/**
 * @brief Clears all pages from the section
 */
void Section::clearPages()
{
    qDeleteAll(m_pages);
    m_pages.clear();
    emit pagesChanged();
}

} // namespace QtWordEditor

