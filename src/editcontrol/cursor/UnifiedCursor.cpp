#include "editcontrol/cursor/UnifiedCursor.h"
#include "core/document/Document.h"
#include "core/document/Block.h"
#include "core/commands/InsertTextCommand.h"
#include "core/commands/RemoveTextCommand.h"
#include "graphics/formula/RowContainerItem.h"
#include "graphics/formula/NumberItem.h"
#include "graphics/formula/FractionItem.h"
#include <QDebug>

namespace QtWordEditor {

UnifiedCursor::UnifiedCursor(Document *document, QObject *parent)
    : QObject(parent)
    , m_document(document)
{
}

UnifiedCursor::~UnifiedCursor()
{
}

// ========== 模式切换 ==========

void UnifiedCursor::setMode(CursorMode mode)
{
    if (m_position.mode != mode) {
        CursorMode oldMode = m_position.mode;
        m_position.mode = mode;
        emit modeChanged(mode, oldMode);
        emit positionChanged(m_position);
    }
}

CursorMode UnifiedCursor::mode() const
{
    return m_position.mode;
}

// ========== 位置管理 ==========

UnifiedCursorPosition UnifiedCursor::position() const
{
    return m_position;
}

void UnifiedCursor::setPosition(const UnifiedCursorPosition &pos)
{
    if (m_position != pos) {
        m_position = pos;
        emit positionChanged(m_position);
    }
}

// ========== 文档模式方法 ==========

void UnifiedCursor::setDocumentPosition(int blockIndex, int offset)
{
    if (m_position.blockIndex != blockIndex || m_position.offset != offset) {
        m_position.blockIndex = blockIndex;
        m_position.offset = offset;
        m_position.mode = CursorMode::DocumentMode;
        m_position.inMathSpan = false;
        m_position.mathSpan = nullptr;
        m_position.mathItem = nullptr;
        m_position.mathContainer = nullptr;
        m_position.mathNumberItem = nullptr;
        emit positionChanged(m_position);
    }
}

void UnifiedCursor::moveLeft()
{
    if (m_position.mode != CursorMode::DocumentMode) {
        return;
    }
    
    // 简化实现：在同一块内移动
    if (m_position.offset > 0) {
        m_position.offset--;
        emit positionChanged(m_position);
    } else if (m_position.blockIndex > 0) {
        // 移动到前一个块的末尾
        Block *prevBlock = m_document->block(m_position.blockIndex - 1);
        if (prevBlock) {
            m_position.blockIndex--;
            m_position.offset = prevBlock->length();
            emit positionChanged(m_position);
        }
    }
}

void UnifiedCursor::moveRight()
{
    if (m_position.mode != CursorMode::DocumentMode) {
        return;
    }
    
    Block *block = m_document->block(m_position.blockIndex);
    if (!block)
        return;
    
    if (m_position.offset < block->length()) {
        m_position.offset++;
        emit positionChanged(m_position);
    } else if (m_position.blockIndex < m_document->blockCount() - 1) {
        // 移动到下一个块的开头
        m_position.blockIndex++;
        m_position.offset = 0;
        emit positionChanged(m_position);
    }
}

void UnifiedCursor::moveUp()
{
    if (m_position.mode != CursorMode::DocumentMode) {
        return;
    }
    
    // 简化实现：移动到上一个块
    if (m_position.blockIndex > 0) {
        m_position.blockIndex--;
        emit positionChanged(m_position);
    }
}

void UnifiedCursor::moveDown()
{
    if (m_position.mode != CursorMode::DocumentMode) {
        return;
    }
    
    if (m_position.blockIndex < m_document->blockCount() - 1) {
        m_position.blockIndex++;
        emit positionChanged(m_position);
    }
}

void UnifiedCursor::moveToStartOfLine()
{
    if (m_position.mode != CursorMode::DocumentMode) {
        return;
    }
    
    m_position.offset = 0;
    emit positionChanged(m_position);
}

void UnifiedCursor::moveToEndOfLine()
{
    if (m_position.mode != CursorMode::DocumentMode) {
        return;
    }
    
    Block *block = m_document->block(m_position.blockIndex);
    if (block) {
        m_position.offset = block->length();
        emit positionChanged(m_position);
    }
}

void UnifiedCursor::moveToStartOfDocument()
{
    if (m_position.mode != CursorMode::DocumentMode) {
        return;
    }
    
    m_position.blockIndex = 0;
    m_position.offset = 0;
    emit positionChanged(m_position);
}

void UnifiedCursor::moveToEndOfDocument()
{
    if (m_position.mode != CursorMode::DocumentMode) {
        return;
    }
    
    int lastBlock = m_document->blockCount() - 1;
    if (lastBlock >= 0) {
        Block *block = m_document->block(lastBlock);
        m_position.blockIndex = lastBlock;
        m_position.offset = block ? block->length() : 0;
        emit positionChanged(m_position);
    }
}

void UnifiedCursor::insertText(const QString &text, const CharacterStyle &style)
{
    if (m_position.mode != CursorMode::DocumentMode) {
        return;
    }
    
    if (!m_document || text.isEmpty())
        return;
    
    QUndoStack *stack = m_document->undoStack();
    if (stack) {
        InsertTextCommand *cmd = new InsertTextCommand(m_document, m_position.blockIndex,
                                                       m_position.offset, text, style);
        stack->push(cmd);
        // 插入后更新光标位置
        m_position.offset += text.length();
        emit positionChanged(m_position);
    }
}

void UnifiedCursor::deletePreviousChar()
{
    if (m_position.mode != CursorMode::DocumentMode) {
        return;
    }
    
    if (!m_document || m_position.offset <= 0)
        return;
    
    QUndoStack *stack = m_document->undoStack();
    if (stack) {
        RemoveTextCommand *cmd = new RemoveTextCommand(m_document, m_position.blockIndex,
                                                        m_position.offset - 1, 1);
        stack->push(cmd);
        m_position.offset--;
        emit positionChanged(m_position);
    }
}

void UnifiedCursor::deleteNextChar()
{
    if (m_position.mode != CursorMode::DocumentMode) {
        return;
    }
    
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
        // offset 保持不变（删除光标后的字符）
    }
}

// ========== 公式模式方法 ==========

void UnifiedCursor::setMathContainerPosition(RowContainerItem *container, int position)
{
    m_position.mode = CursorMode::MathContainerMode;
    m_position.mathContainer = container;
    m_position.mathNumberItem = nullptr;
    m_position.mathChildIndex = position;
    m_position.inMathSpan = true;
    emit positionChanged(m_position);
}

void UnifiedCursor::setMathNumberPosition(NumberItem *numberItem, int position)
{
    m_position.mode = CursorMode::MathNumberMode;
    m_position.mathNumberItem = numberItem;
    m_position.mathContainer = nullptr;
    m_position.mathChildOffset = position;
    m_position.inMathSpan = true;
    emit positionChanged(m_position);
}

void UnifiedCursor::mathMoveLeft()
{
    if (m_position.mode == CursorMode::MathNumberMode && m_position.mathNumberItem) {
        if (m_position.mathChildOffset > 0) {
            m_position.mathChildOffset--;
            emit positionChanged(m_position);
        }
    } else if (m_position.mode == CursorMode::MathContainerMode && m_position.mathContainer) {
        if (m_position.mathChildIndex > 0) {
            m_position.mathChildIndex--;
            emit positionChanged(m_position);
        }
    }
}

void UnifiedCursor::mathMoveRight()
{
    if (m_position.mode == CursorMode::MathNumberMode && m_position.mathNumberItem) {
        int maxPos = m_position.mathNumberItem->textLength();
        if (m_position.mathChildOffset < maxPos) {
            m_position.mathChildOffset++;
            emit positionChanged(m_position);
        }
    } else if (m_position.mode == CursorMode::MathContainerMode && m_position.mathContainer) {
        int maxPos = m_position.mathContainer->childCount();
        if (m_position.mathChildIndex < maxPos) {
            m_position.mathChildIndex++;
            emit positionChanged(m_position);
        }
    }
}

void UnifiedCursor::mathMoveUp()
{
    // 检查是否在 NumberItem 模式下
    if (m_position.mode != CursorMode::MathNumberMode || !m_position.mathNumberItem) {
        return;
    }
    
    // 查找父级是否是 FractionItem
    QGraphicsItem *parent = m_position.mathNumberItem->parentItem();
    while (parent) {
        FractionItem *fracItem = dynamic_cast<FractionItem*>(parent);
        if (fracItem) {
            // 检查当前是在分母还是分子
            if (m_position.mathNumberItem == fracItem->denominatorItem()) {
                // 当前在分母，移动到分子
                NumberItem *numItem = dynamic_cast<NumberItem*>(fracItem->numeratorItem());
                if (numItem) {
                    // 计算相同的字符位置（或边界）
                    int newPos = qMin(m_position.mathChildOffset, numItem->textLength());
                    setMathNumberPosition(numItem, newPos);
                }
            }
            return;
        }
        parent = parent->parentItem();
    }
}

void UnifiedCursor::mathMoveDown()
{
    // 检查是否在 NumberItem 模式下
    if (m_position.mode != CursorMode::MathNumberMode || !m_position.mathNumberItem) {
        return;
    }
    
    // 查找父级是否是 FractionItem
    QGraphicsItem *parent = m_position.mathNumberItem->parentItem();
    while (parent) {
        FractionItem *fracItem = dynamic_cast<FractionItem*>(parent);
        if (fracItem) {
            // 检查当前是在分子还是分母
            if (m_position.mathNumberItem == fracItem->numeratorItem()) {
                // 当前在分子，移动到分母
                NumberItem *denItem = dynamic_cast<NumberItem*>(fracItem->denominatorItem());
                if (denItem) {
                    // 计算相同的字符位置（或边界）
                    int newPos = qMin(m_position.mathChildOffset, denItem->textLength());
                    setMathNumberPosition(denItem, newPos);
                }
            }
            return;
        }
        parent = parent->parentItem();
    }
}

void UnifiedCursor::mathMoveToParent()
{
    // 移动到父容器（暂时不实现）
}

// ========== 退出公式模式 ==========

void UnifiedCursor::exitMathMode()
{
    if (m_position.mode != CursorMode::DocumentMode) {
        m_position.mode = CursorMode::DocumentMode;
        m_position.inMathSpan = false;
        m_position.mathSpan = nullptr;
        m_position.mathItem = nullptr;
        m_position.mathContainer = nullptr;
        m_position.mathNumberItem = nullptr;
        emit positionChanged(m_position);
    }
}

} // namespace QtWordEditor
