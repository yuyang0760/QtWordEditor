#include "core/document/Document.h"
#include "core/document/Section.h"
#include "core/document/Block.h"
#include <QUndoStack>
#include <QDebug>

namespace QtWordEditor {

Document::Document(QObject *parent)
    : QObject(parent)
    , m_created(QDateTime::currentDateTime())
    , m_modified(m_created)
    , m_undoStack(new QUndoStack(this))
{
}

Document::~Document()
{
    qDeleteAll(m_sections);
}

QString Document::title() const
{
    return m_title;
}

void Document::setTitle(const QString &title)
{
    if (m_title != title) {
        m_title = title;
        emit documentChanged();
    }
}

QString Document::author() const
{
    return m_author;
}

void Document::setAuthor(const QString &author)
{
    if (m_author != author) {
        m_author = author;
        emit documentChanged();
    }
}

QDateTime Document::created() const
{
    return m_created;
}

QDateTime Document::modified() const
{
    return m_modified;
}

void Document::setModified(const QDateTime &modified)
{
    if (m_modified != modified) {
        m_modified = modified;
        emit documentChanged();
    }
}

int Document::sectionCount() const
{
    return m_sections.size();
}

Section *Document::section(int index) const
{
    if (index >= 0 && index < m_sections.size())
        return m_sections.at(index);
    return nullptr;
}

void Document::addSection(Section *section)
{
    insertSection(m_sections.size(), section);
}

void Document::insertSection(int index, Section *section)
{
    if (!section || index < 0 || index > m_sections.size())
        return;
    section->setParent(this);
    m_sections.insert(index, section);
    emit sectionAdded(index);
    updateBlockPositions();
}

void Document::removeSection(int index)
{
    if (index < 0 || index >= m_sections.size())
        return;
    Section *section = m_sections.takeAt(index);
    section->deleteLater();
    emit sectionRemoved(index);
    updateBlockPositions();
}

int Document::blockCount() const
{
    int total = 0;
    for (Section *section : m_sections)
        total += section->blockCount();
    return total;
}

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

QUndoStack *Document::undoStack() const
{
    return m_undoStack.data();
}

QString Document::plainText() const
{
    QString result;
    for (Section *section : m_sections) {
        for (int i = 0; i < section->blockCount(); ++i) {
            Block *blk = section->block(i);
            // TODO: implement text extraction for each block type
            result += "[block]";
        }
        result += "\n";
    }
    return result;
}

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