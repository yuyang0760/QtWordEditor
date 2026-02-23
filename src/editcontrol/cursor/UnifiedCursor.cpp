/**
 * @file UnifiedCursor.cpp
 * @brief 统一光标类实现
 * 
 * 内部使用新的坐标路径系统，外部保持旧 API 兼容
 */

#include "editcontrol/cursor/UnifiedCursor.h"
#include "editcontrol/cursor/CursorPositionAdapter.h"
#include "core/document/Document.h"
#include "core/document/Block.h"
#include "core/commands/InsertTextCommand.h"
#include "core/commands/RemoveTextCommand.h"
#include "graphics/formula/RowContainerItem.h"
#include "graphics/formula/NumberItem.h"
#include "graphics/formula/FractionItem.h"
#include <QDebug>

namespace QtWordEditor {

/**
 * @brief 构造函数
 * @param document 关联的文档
 * @param parent 父对象指针
 */
UnifiedCursor::UnifiedCursor(Document *document, QObject *parent)
    : QObject(parent)
    , m_document(document)
{
}

/**
 * @brief 析构函数
 */
UnifiedCursor::~UnifiedCursor()
{
}

// ========== 模式切换 ==========

/**
 * @brief 设置光标模式
 * @param mode 新的光标模式
 */
void UnifiedCursor::setMode(CursorMode mode)
{
    // 转换当前位置为旧格式以获取当前模式
    UnifiedCursorPosition oldPos = CursorPositionAdapter::toOld(m_newPosition);
    if (oldPos.mode != mode) {
        CursorMode oldMode = oldPos.mode;
        
        // 根据新模式调整新位置
        if (mode == CursorMode::DocumentMode) {
            m_newPosition.mathPath = std::nullopt;
        }
        
        emit modeChanged(mode, oldMode);
        emit positionChanged(position());
    }
}

/**
 * @brief 获取当前光标模式
 * @return 当前光标模式
 */
CursorMode UnifiedCursor::mode() const
{
    UnifiedCursorPosition oldPos = CursorPositionAdapter::toOld(m_newPosition);
    return oldPos.mode;
}

// ========== 位置管理 ==========

/**
 * @brief 获取当前光标位置
 * @return 当前光标位置结构体
 */
UnifiedCursorPosition UnifiedCursor::position() const
{
    return CursorPositionAdapter::toOld(m_newPosition);
}

/**
 * @brief 设置光标位置
 * @param pos 光标位置结构体
 */
void UnifiedCursor::setPosition(const UnifiedCursorPosition &pos)
{
    NewCursorPosition newPos = CursorPositionAdapter::toNew(pos);
    if (m_newPosition != newPos) {
        m_newPosition = newPos;
        emit positionChanged(pos);
    }
}

// ========== 文档模式方法 ==========

/**
 * @brief 设置文档模式光标位置
 * @param blockIndex 块索引
 * @param offset 块内偏移量
 */
void UnifiedCursor::setDocumentPosition(int blockIndex, int offset)
{
    if (m_newPosition.blockIndex != blockIndex || m_newPosition.offset != offset) {
        m_newPosition.blockIndex = blockIndex;
        m_newPosition.offset = offset;
        m_newPosition.mathPath = std::nullopt;
        emit positionChanged(position());
    }
}

/**
 * @brief 向左移动光标
 */
void UnifiedCursor::moveLeft()
{
    if (!m_newPosition.isDocumentMode()) {
        return;
    }
    
    // 在同一块内移动
    if (m_newPosition.offset > 0) {
        m_newPosition.offset--;
        emit positionChanged(position());
    } else if (m_newPosition.blockIndex > 0) {
        // 移动到前一个块的末尾
        Block *prevBlock = m_document->block(m_newPosition.blockIndex - 1);
        if (prevBlock) {
            m_newPosition.blockIndex--;
            m_newPosition.offset = prevBlock->length();
            emit positionChanged(position());
        }
    }
}

/**
 * @brief 向右移动光标
 */
void UnifiedCursor::moveRight()
{
    if (!m_newPosition.isDocumentMode()) {
        return;
    }
    
    Block *block = m_document->block(m_newPosition.blockIndex);
    if (!block)
        return;
    
    if (m_newPosition.offset < block->length()) {
        m_newPosition.offset++;
        emit positionChanged(position());
    } else if (m_newPosition.blockIndex < m_document->blockCount() - 1) {
        // 移动到下一个块的开头
        m_newPosition.blockIndex++;
        m_newPosition.offset = 0;
        emit positionChanged(position());
    }
}

/**
 * @brief 向上移动光标
 */
void UnifiedCursor::moveUp()
{
    if (!m_newPosition.isDocumentMode()) {
        return;
    }
    
    // 移动到上一个块
    if (m_newPosition.blockIndex > 0) {
        m_newPosition.blockIndex--;
        emit positionChanged(position());
    }
}

/**
 * @brief 向下移动光标
 */
void UnifiedCursor::moveDown()
{
    if (!m_newPosition.isDocumentMode()) {
        return;
    }
    
    if (m_newPosition.blockIndex < m_document->blockCount() - 1) {
        m_newPosition.blockIndex++;
        emit positionChanged(position());
    }
}

/**
 * @brief 移动到行首
 */
void UnifiedCursor::moveToStartOfLine()
{
    if (!m_newPosition.isDocumentMode()) {
        return;
    }
    
    m_newPosition.offset = 0;
    emit positionChanged(position());
}

/**
 * @brief 移动到行尾
 */
void UnifiedCursor::moveToEndOfLine()
{
    if (!m_newPosition.isDocumentMode()) {
        return;
    }
    
    Block *block = m_document->block(m_newPosition.blockIndex);
    if (block) {
        m_newPosition.offset = block->length();
        emit positionChanged(position());
    }
}

/**
 * @brief 移动到文档开头
 */
void UnifiedCursor::moveToStartOfDocument()
{
    if (!m_newPosition.isDocumentMode()) {
        return;
    }
    
    m_newPosition.blockIndex = 0;
    m_newPosition.offset = 0;
    emit positionChanged(position());
}

/**
 * @brief 移动到文档结尾
 */
void UnifiedCursor::moveToEndOfDocument()
{
    if (!m_newPosition.isDocumentMode()) {
        return;
    }
    
    int lastBlock = m_document->blockCount() - 1;
    if (lastBlock >= 0) {
        Block *block = m_document->block(lastBlock);
        m_newPosition.blockIndex = lastBlock;
        m_newPosition.offset = block ? block->length() : 0;
        emit positionChanged(position());
    }
}

/**
 * @brief 在光标位置插入文本
 * @param text 要插入的文本
 * @param style 文本的字符样式
 */
void UnifiedCursor::insertText(const QString &text, const CharacterStyle &style)
{
    if (!m_newPosition.isDocumentMode()) {
        return;
    }
    
    if (!m_document || text.isEmpty())
        return;
    
    QUndoStack *stack = m_document->undoStack();
    if (stack) {
        InsertTextCommand *cmd = new InsertTextCommand(m_document, m_newPosition.blockIndex,
                                                       m_newPosition.offset, text, style);
        stack->push(cmd);
        // 插入后更新光标位置
        m_newPosition.offset += text.length();
        emit positionChanged(position());
    }
}

/**
 * @brief 删除光标前一个字符
 */
void UnifiedCursor::deletePreviousChar()
{
    if (!m_newPosition.isDocumentMode()) {
        return;
    }
    
    if (!m_document || m_newPosition.offset <= 0)
        return;
    
    QUndoStack *stack = m_document->undoStack();
    if (stack) {
        RemoveTextCommand *cmd = new RemoveTextCommand(m_document, m_newPosition.blockIndex,
                                                        m_newPosition.offset - 1, 1);
        stack->push(cmd);
        m_newPosition.offset--;
        emit positionChanged(position());
    }
}

/**
 * @brief 删除光标后一个字符
 */
void UnifiedCursor::deleteNextChar()
{
    if (!m_newPosition.isDocumentMode()) {
        return;
    }
    
    if (!m_document)
        return;
    
    Block *block = m_document->block(m_newPosition.blockIndex);
    if (!block || m_newPosition.offset >= block->length())
        return;
    
    QUndoStack *stack = m_document->undoStack();
    if (stack) {
        RemoveTextCommand *cmd = new RemoveTextCommand(m_document, m_newPosition.blockIndex,
                                                        m_newPosition.offset, 1);
        stack->push(cmd);
        // offset 保持不变（删除光标后的字符）
    }
}

// ========== 公式模式方法 ==========

/**
 * @brief 设置公式容器模式光标位置
 * @param container 光标所在的行容器
 * @param pos 光标在容器中的位置索引
 */
void UnifiedCursor::setMathContainerPosition(RowContainerItem *container, int pos)
{
    // 创建临时的旧位置，然后转换为新位置
    UnifiedCursorPosition oldPos;
    oldPos.blockIndex = m_newPosition.blockIndex;
    oldPos.offset = m_newPosition.offset;
    oldPos.mode = CursorMode::MathContainerMode;
    oldPos.mathContainer = container;
    oldPos.mathChildIndex = pos;
    oldPos.inMathSpan = true;
    
    m_newPosition = CursorPositionAdapter::toNew(oldPos);
    emit positionChanged(this->position());
}

/**
 * @brief 设置公式数字模式光标位置
 * @param numberItem 光标所在的数字项
 * @param pos 光标在数字中的字符偏移
 */
void UnifiedCursor::setMathNumberPosition(NumberItem *numberItem, int pos)
{
    // 创建临时的旧位置，然后转换为新位置
    UnifiedCursorPosition oldPos;
    oldPos.blockIndex = m_newPosition.blockIndex;
    oldPos.offset = m_newPosition.offset;
    oldPos.mode = CursorMode::MathNumberMode;
    oldPos.mathNumberItem = numberItem;
    oldPos.mathChildOffset = pos;
    oldPos.inMathSpan = true;
    
    m_newPosition = CursorPositionAdapter::toNew(oldPos);
    emit positionChanged(this->position());
}

/**
 * @brief 在公式中向左移动光标
 */
void UnifiedCursor::mathMoveLeft()
{
    UnifiedCursorPosition oldPos = CursorPositionAdapter::toOld(m_newPosition);
    
    if (oldPos.mode == CursorMode::MathNumberMode && oldPos.mathNumberItem) {
        if (oldPos.mathChildOffset > 0) {
            oldPos.mathChildOffset--;
            m_newPosition = CursorPositionAdapter::toNew(oldPos);
            emit positionChanged(position());
        }
    } else if (oldPos.mode == CursorMode::MathContainerMode && oldPos.mathContainer) {
        if (oldPos.mathChildIndex > 0) {
            oldPos.mathChildIndex--;
            m_newPosition = CursorPositionAdapter::toNew(oldPos);
            emit positionChanged(position());
        }
    }
}

/**
 * @brief 在公式中向右移动光标
 */
void UnifiedCursor::mathMoveRight()
{
    UnifiedCursorPosition oldPos = CursorPositionAdapter::toOld(m_newPosition);
    
    if (oldPos.mode == CursorMode::MathNumberMode && oldPos.mathNumberItem) {
        int maxPos = oldPos.mathNumberItem->textLength();
        if (oldPos.mathChildOffset < maxPos) {
            oldPos.mathChildOffset++;
            m_newPosition = CursorPositionAdapter::toNew(oldPos);
            emit positionChanged(position());
        }
    } else if (oldPos.mode == CursorMode::MathContainerMode && oldPos.mathContainer) {
        int maxPos = oldPos.mathContainer->childCount();
        if (oldPos.mathChildIndex < maxPos) {
            oldPos.mathChildIndex++;
            m_newPosition = CursorPositionAdapter::toNew(oldPos);
            emit positionChanged(position());
        }
    }
}

/**
 * @brief 在公式中向上移动光标
 */
void UnifiedCursor::mathMoveUp()
{
    UnifiedCursorPosition oldPos = CursorPositionAdapter::toOld(m_newPosition);
    
    // 检查是否在 NumberItem 模式下
    if (oldPos.mode != CursorMode::MathNumberMode || !oldPos.mathNumberItem) {
        return;
    }
    
    // 查找父级是否是 FractionItem
    QGraphicsItem *parent = oldPos.mathNumberItem->parentItem();
    while (parent) {
        FractionItem *fracItem = dynamic_cast<FractionItem*>(parent);
        if (fracItem) {
            // 检查当前是在分母还是分子
            if (oldPos.mathNumberItem == fracItem->denominatorItem()) {
                // 当前在分母，移动到分子
                NumberItem *numItem = dynamic_cast<NumberItem*>(fracItem->numeratorItem());
                if (numItem) {
                    // 计算相同的字符位置（或边界）
                    int newPos = qMin(oldPos.mathChildOffset, numItem->textLength());
                    setMathNumberPosition(numItem, newPos);
                }
            }
            return;
        }
        parent = parent->parentItem();
    }
}

/**
 * @brief 在公式中向下移动光标
 */
void UnifiedCursor::mathMoveDown()
{
    UnifiedCursorPosition oldPos = CursorPositionAdapter::toOld(m_newPosition);
    
    // 检查是否在 NumberItem 模式下
    if (oldPos.mode != CursorMode::MathNumberMode || !oldPos.mathNumberItem) {
        return;
    }
    
    // 查找父级是否是 FractionItem
    QGraphicsItem *parent = oldPos.mathNumberItem->parentItem();
    while (parent) {
        FractionItem *fracItem = dynamic_cast<FractionItem*>(parent);
        if (fracItem) {
            // 检查当前是在分子还是分母
            if (oldPos.mathNumberItem == fracItem->numeratorItem()) {
                // 当前在分子，移动到分母
                NumberItem *denItem = dynamic_cast<NumberItem*>(fracItem->denominatorItem());
                if (denItem) {
                    // 计算相同的字符位置（或边界）
                    int newPos = qMin(oldPos.mathChildOffset, denItem->textLength());
                    setMathNumberPosition(denItem, newPos);
                }
            }
            return;
        }
        parent = parent->parentItem();
    }
}

/**
 * @brief 移动到父容器（暂时不实现）
 */
void UnifiedCursor::mathMoveToParent()
{
    // 移动到父容器（暂时不实现）
}

// ========== 退出公式模式 ==========

/**
 * @brief 退出公式模式，回到文档模式
 */
void UnifiedCursor::exitMathMode()
{
    if (m_newPosition.isMathMode()) {
        m_newPosition.mathPath = std::nullopt;
        emit positionChanged(position());
    }
}

} // namespace QtWordEditor
