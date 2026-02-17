#include "include/editcontrol/cursor/Cursor.h"
#include "include/core/document/Document.h"
#include "include/core/document/Block.h"
#include "include/core/commands/InsertTextCommand.h"
#include "include/core/commands/RemoveTextCommand.h"
#include <QDebug>

namespace QtWordEditor {

Cursor::Cursor(Document *document, QObject *parent)
    : QObject(parent)
    , m_document(document)
{
}

Cursor::~Cursor()
{
}

Document *Cursor::document() const
{
    return m_document;
}

CursorPosition Cursor::position() const
{
    return m_position;
}

void Cursor::setPosition(int blockIndex, int offset)
{
    CursorPosition newPos{blockIndex, offset};
    if (m_position != newPos) {
        m_position = newPos;
        emit positionChanged(m_position);
    }
}

void Cursor::setPosition(const CursorPosition &pos)
{
    if (m_position != pos) {
        m_position = pos;
        emit positionChanged(m_position);
    }
}

void Cursor::moveLeft()
{
    // Simplified: move within same block
    if (m_position.offset > 0) {
        m_position.offset--;
        emit positionChanged(m_position);
    } else if (m_position.blockIndex > 0) {
        // Move to previous block, end of that block
        Block *prevBlock = m_document->block(m_position.blockIndex - 1);
        if (prevBlock) {
            m_position.blockIndex--;
            m_position.offset = prevBlock->length();
            emit positionChanged(m_position);
        }
    }
}

void Cursor::moveRight()
{
    Block *block = m_document->block(m_position.blockIndex);
    if (!block)
        return;
    if (m_position.offset < block->length()) {
        m_position.offset++;
        emit positionChanged(m_position);
    } else if (m_position.blockIndex < m_document->blockCount() - 1) {
        // Move to next block, start
        m_position.blockIndex++;
        m_position.offset = 0;
        emit positionChanged(m_position);
    }
}

void Cursor::moveUp()
{
    // Placeholder: just move to previous line (simplified)
    // TODO: implement proper line‑based movement
    if (m_position.blockIndex > 0) {
        m_position.blockIndex--;
        emit positionChanged(m_position);
    }
}

void Cursor::moveDown()
{
    if (m_position.blockIndex < m_document->blockCount() - 1) {
        m_position.blockIndex++;
        emit positionChanged(m_position);
    }
}

void Cursor::moveToStartOfLine()
{
    m_position.offset = 0;
    emit positionChanged(m_position);
}

void Cursor::moveToEndOfLine()
{
    Block *block = m_document->block(m_position.blockIndex);
    if (block) {
        m_position.offset = block->length();
        emit positionChanged(m_position);
    }
}

void Cursor::moveToStartOfDocument()
{
    m_position.blockIndex = 0;
    m_position.offset = 0;
    emit positionChanged(m_position);
}

void Cursor::moveToEndOfDocument()
{
    int lastBlock = m_document->blockCount() - 1;
    if (lastBlock >= 0) {
        Block *block = m_document->block(lastBlock);
        m_position.blockIndex = lastBlock;
        m_position.offset = block ? block->length() : 0;
        emit positionChanged(m_position);
    }
}

void Cursor::insertText(const QString &text, const CharacterStyle &style)
{
    if (!m_document || text.isEmpty())
        return;
    QUndoStack *stack = m_document->undoStack();
    if (stack) {
        InsertTextCommand *cmd = new InsertTextCommand(m_document, m_position.blockIndex,
                                                       m_position.offset, text, style);
        stack->push(cmd);
        // Update cursor position after insertion
        m_position.offset += text.length();
        emit positionChanged(m_position);
    }
}

void Cursor::deletePreviousChar()
{
    if (!m_document || m_position.offset <= 0)
        return;
    // Remove one character before cursor
    QUndoStack *stack = m_document->undoStack();
    if (stack) {
        RemoveTextCommand *cmd = new RemoveTextCommand(m_document, m_position.blockIndex,
                                                        m_position.offset - 1, 1);
        stack->push(cmd);
        m_position.offset--;
        emit positionChanged(m_position);
    }
}

void Cursor::deleteNextChar()
{
    if (!m_document)
        return;
    Block *block = m_document->block(m_position.blockIndex);
    if (!block || m_position.offset >= block->length())
        return;
    QUndoStack *stack = m_document->undoStack();
    if (stack) {
        RemoveTextCommand *cmd = new RemoveTextCommand(m_document, m_position.blockIndex,
                                                        m_position.offset, 1);
        stack->push(cmd);
        // offset stays the same (character after cursor removed)
    }
}

} // namespace QtWordEditor