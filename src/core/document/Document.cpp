
/**
 * @file Document.cpp
 * @brief 文档类的实现文件
 *
 * 此文件包含了文档类的完整实现，该类是整个文档系统的根容器，
 * 负责管理节、元数据以及撤销重做功能。
 */

#include "core/document/Document.h"
#include "core/document/Section.h"
#include "core/document/Block.h"
#include <QUndoStack>
#include <QDebug>

namespace QtWordEditor {

/**
 * @brief 构造文档对象
 * @param parent 父QObject对象
 */
Document::Document(QObject *parent)
    : QObject(parent)
    , m_created(QDateTime::currentDateTime())
    , m_modified(m_created)
    , m_undoStack(new QUndoStack(this))
{
}

/**
 * @brief 销毁文档对象并清理所有节资源
 */
Document::~Document()
{
    qDeleteAll(m_sections);
}

/**
 * @brief 获取文档标题
 * @return 文档标题字符串
 */
QString Document::title() const
{
    return m_title;
}

/**
 * @brief 设置文档标题
 * @param title 新的文档标题
 */
void Document::setTitle(const QString &title)
{
    if (m_title != title) {
        m_title = title;
        emit documentChanged();
    }
}

/**
 * @brief 获取文档作者
 * @return 作者姓名字符串
 */
QString Document::author() const
{
    return m_author;
}

/**
 * @brief Sets the document author
 * @param author New author name
 */
void Document::setAuthor(const QString &author)
{
    if (m_author != author) {
        m_author = author;
        emit documentChanged();
    }
}

/**
 * @brief 获取文档创建时间戳
 * @return 文档创建时间
 */
QDateTime Document::created() const
{
    return m_created;
}

/**
 * @brief 获取文档修改时间戳
 * @return 文档最后修改时间
 */
QDateTime Document::modified() const
{
    return m_modified;
}

/**
 * @brief Sets the document modification timestamp
 * @param modified New modification datetime
 */
void Document::setModified(const QDateTime &modified)
{
    if (m_modified != modified) {
        m_modified = modified;
        emit documentChanged();
    }
}

/**
 * @brief 获取文档中节的数量
 * @return 节的数量
 */
int Document::sectionCount() const
{
    return m_sections.size();
}

/**
 * @brief Gets a section by index
 * @param index Section index
 * @return Pointer to Section, or nullptr if index is invalid
 */
Section *Document::section(int index) const
{
    if (index >= 0 && index < m_sections.size())
        return m_sections.at(index);
    return nullptr;
}

/**
 * @brief 在文档末尾添加节
 * @param section 要添加的节
 */
void Document::addSection(Section *section)
{
    insertSection(m_sections.size(), section);
}

/**
 * @brief Inserts a section at the specified index
 * @param index Insert position
 * @param section Section to insert
 */
void Document::insertSection(int index, Section *section)
{
    if (!section || index < 0 || index > m_sections.size())
        return;
    section->setParent(this);
    m_sections.insert(index, section);
    emit sectionAdded(index);
    updateBlockPositions();
}

/**
 * @brief Removes a section at the specified index
 * @param index Section index to remove
 */
void Document::removeSection(int index)
{
    if (index < 0 || index >= m_sections.size())
        return;
    Section *section = m_sections.takeAt(index);
    section->deleteLater();
    emit sectionRemoved(index);
    updateBlockPositions();
}

/**
 * @brief Gets the total number of blocks across all sections
 * @return Total block count
 */
int Document::blockCount() const
{
    int total = 0;
    for (Section *section : m_sections)
        total += section->blockCount();
    return total;
}

/**
 * @brief Gets a block by global index across all sections
 * @param globalIndex Global block index
 * @return Pointer to Block, or nullptr if index is invalid
 */
Block *Document::block(int globalIndex) const
{
    if (globalIndex < 0)
        return nullptr;
    int remaining = globalIndex;
    for (Section *section : m_sections) {
        int cnt = section->blockCount();
        if (remaining < cnt)
            return section->block(remaining);
        remaining -= cnt;
    }
    return nullptr;
}

/**
 * @brief 获取文档的撤销栈
 * @return 指向QUndoStack的指针
 */
QUndoStack *Document::undoStack() const
{
    return m_undoStack.data();
}

/**
 * @brief Exports the document as plain text (for testing)
 * @return Plain text representation
 */
QString Document::plainText() const
{
    QString result;
    for (Section *section : m_sections) {
        for (int i = 0; i < section->blockCount(); ++i) {
            Block *blk = section->block(i);
            result += "[block]";
        }
        result += "\n";
    }
    return result;
}

/**
 * @brief Updates the global position of all blocks in the document
 */
void Document::updateBlockPositions()
{
    int globalIndex = 0;
    for (Section *section : m_sections) {
        for (int i = 0; i < section->blockCount(); ++i) {
            Block *blk = section->block(i);
            blk->setPositionInDocument(globalIndex++);
        }
    }
}

} // namespace QtWordEditor

