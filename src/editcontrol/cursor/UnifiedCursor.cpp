/**
 * @file UnifiedCursor.cpp
 * @brief 统一光标类实现（无模式版本）
 * 
 * 完全采用坐标路径系统，不再区分模式
 */

#include "editcontrol/cursor/UnifiedCursor.h"
#include "editcontrol/cursor/CursorPositionAdapter.h"
#include "core/document/Document.h"
#include "core/document/Block.h"
#include "core/commands/InsertTextCommand.h"
#include "core/commands/RemoveTextCommand.h"
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

// ========== 文档位置方法 ==========

/**
 * @brief 设置文档光标位置
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

// ========== 公式位置方法 ==========

/**
 * @brief 设置公式光标位置
 * @param mathPath 公式坐标路径
 */
void UnifiedCursor::setMathPosition(const CoordinatePath &mathPath)
{
    if (mathPath.isValid() && !mathPath.isEmpty()) {
        m_newPosition.mathPath = mathPath;
        emit positionChanged(position());
    }
}

/**
 * @brief 在公式中向左移动光标
 */
void UnifiedCursor::mathMoveLeft()
{
    if (!m_newPosition.isMathMode() || !m_newPosition.mathPath->isValid() || m_newPosition.mathPath->isEmpty()) {
        return;
    }
    
    // 在公式中向左移动（简化实现，后续根据具体需求完善）
    CoordinatePath &path = *m_newPosition.mathPath;
    PathSegment &lastSegment = path.top();
    
    if (lastSegment.childOffset > 0) {
        lastSegment.childOffset--;
        emit positionChanged(position());
    } else if (lastSegment.childIndex > 0) {
        lastSegment.childIndex--;
        emit positionChanged(position());
    }
}

/**
 * @brief 在公式中向右移动光标
 */
void UnifiedCursor::mathMoveRight()
{
    if (!m_newPosition.isMathMode() || !m_newPosition.mathPath->isValid() || m_newPosition.mathPath->isEmpty()) {
        return;
    }
    
    // 在公式中向右移动（简化实现，后续根据具体需求完善）
    CoordinatePath &path = *m_newPosition.mathPath;
    PathSegment &lastSegment = path.top();
    
    lastSegment.childOffset++;
    emit positionChanged(position());
}

/**
 * @brief 在公式中向上移动光标
 */
void UnifiedCursor::mathMoveUp()
{
    if (!m_newPosition.isMathMode() || !m_newPosition.mathPath->isValid() || m_newPosition.mathPath->isEmpty()) {
        return;
    }
    
    CoordinatePath &path = *m_newPosition.mathPath;
    PathSegment &lastSegment = path.top();
    MathItem *currentItem = lastSegment.container;
    
    if (!currentItem) {
        return;
    }
    
    // 查找父级是否是 FractionItem
    MathItem *parent = currentItem->parentMathItem();
    while (parent) {
        // 检查是否是 FractionItem（通过 type() 判断）
        if (parent->type() == QGraphicsItem::UserType + 2003) {
            // 获取分子和分母
            MathItem *numerator = nullptr;
            MathItem *denominator = nullptr;
            
            // 通过 childAt() 获取分子（0）和分母（1）
            if (parent->childCount() >= 2) {
                numerator = parent->childAt(0);
                denominator = parent->childAt(1);
            }
            
            // 检查当前是在分母还是分子
            if (currentItem == denominator && numerator) {
                // 当前在分母，移动到分子
                // 先弹出当前项
                path.pop();
                // 添加分子项
                path.push(PathSegment(numerator, 0, lastSegment.childOffset));
                emit positionChanged(position());
            }
            return;
        }
        parent = parent->parentMathItem();
    }
}

/**
 * @brief 在公式中向下移动光标
 */
void UnifiedCursor::mathMoveDown()
{
    if (!m_newPosition.isMathMode() || !m_newPosition.mathPath->isValid() || m_newPosition.mathPath->isEmpty()) {
        return;
    }
    
    CoordinatePath &path = *m_newPosition.mathPath;
    PathSegment &lastSegment = path.top();
    MathItem *currentItem = lastSegment.container;
    
    if (!currentItem) {
        return;
    }
    
    // 查找父级是否是 FractionItem
    MathItem *parent = currentItem->parentMathItem();
    while (parent) {
        // 检查是否是 FractionItem（通过 type() 判断）
        if (parent->type() == QGraphicsItem::UserType + 2003) {
            // 获取分子和分母
            MathItem *numerator = nullptr;
            MathItem *denominator = nullptr;
            
            // 通过 childAt() 获取分子（0）和分母（1）
            if (parent->childCount() >= 2) {
                numerator = parent->childAt(0);
                denominator = parent->childAt(1);
            }
            
            // 检查当前是在分子还是分母
            if (currentItem == numerator && denominator) {
                // 当前在分子，移动到分母
                // 先弹出当前项
                path.pop();
                // 添加分母项
                path.push(PathSegment(denominator, 0, lastSegment.childOffset));
                emit positionChanged(position());
            }
            return;
        }
        parent = parent->parentMathItem();
    }
}

/**
 * @brief 移动到公式父容器
 */
void UnifiedCursor::mathMoveToParent()
{
    if (!m_newPosition.isMathMode() || !m_newPosition.mathPath->isValid() || m_newPosition.mathPath->depth() <= 1) {
        return;
    }
    
    // 弹出最后一段路径，移动到父容器
    m_newPosition.mathPath->pop();
    emit positionChanged(position());
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
