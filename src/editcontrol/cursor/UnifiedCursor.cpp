
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
    // 初始化位置
    m_position.blockIndex = 0;
    m_position.offset = 0;
    // mathPath 初始化为空
}

/**
 * @brief 析构函数
 */
UnifiedCursor::~UnifiedCursor()
{
}

/**
 * @brief 获取关联的文档
 * @return 当前文档指针
 */
Document *UnifiedCursor::document() const
{
    return m_document;
}

// ========== 位置管理（旧接口 - 兼容 Cursor） ==========

/**
 * @brief 获取当前光标位置（旧格式）
 * @return 当前光标位置结构体
 */
CursorPosition UnifiedCursor::position() const
{
    return CursorPositionAdapter::unifiedToCursor(unifiedPosition());
}

/**
 * @brief 设置光标位置（旧格式）
 * @param blockIndex 块索引
 * @param offset 块内偏移量
 */
void UnifiedCursor::setPosition(int blockIndex, int offset)
{
    if (m_position.blockIndex != blockIndex || m_position.offset != offset) {
        m_position.blockIndex = blockIndex;
        m_position.offset = offset;
        m_position.mathPath = std::nullopt;
        m_position.mathTextOffset = 0;
        emitPositionChangedSignals();
    }
}

/**
 * @brief 设置光标位置（旧格式）
 * @param pos 光标位置结构体
 */
void UnifiedCursor::setPosition(const CursorPosition &pos)
{
    UnifiedCursorPosition unifiedPos = CursorPositionAdapter::cursorToUnified(pos);
    if (m_position != unifiedPos) {
        m_position = unifiedPos;
        emitPositionChangedSignals();
    }
}

// ========== 位置管理（统一接口） ==========

/**
 * @brief 获取当前光标位置
 * @return 当前光标位置结构体
 */
UnifiedCursorPosition UnifiedCursor::unifiedPosition() const
{
    return m_position;
}

/**
 * @brief 设置光标位置
 * @param pos 光标位置结构体
 */
void UnifiedCursor::setUnifiedPosition(const UnifiedCursorPosition &pos)
{
    qDebug() << "[UnifiedCursor::setUnifiedPosition] 开始";
    
    qDebug() << "  旧位置 blockIndex:" << m_position.blockIndex << " offset:" << m_position.offset << " isMath:" << m_position.isMathMode();
    qDebug() << "  新位置 blockIndex:" << pos.blockIndex << " offset:" << pos.offset << " isMath:" << pos.isMathMode();
    qDebug() << "  pos.mathPath.has_value():" << pos.mathPath.has_value();
    if (pos.mathPath.has_value()) {
        qDebug() << "  pos.mathPath depth:" << pos.mathPath->depth();
        qDebug() << "  pos.mathTextOffset:" << pos.mathTextOffset;
    }
    
    if (m_position != pos) {
        qDebug() << "  位置不同，更新并发出信号";
        m_position = pos;
        
        qDebug() << "  更新后 m_position.isMathMode():" << m_position.isMathMode();
        qDebug() << "  更新后 m_position.mathPath.has_value():" << m_position.mathPath.has_value();
        
        emitPositionChangedSignals();
    } else {
        qDebug() << "  位置相同，不更新";
    }
}

/**
 * @brief 在内部位置变化时，同时发出新旧格式的信号
 */
void UnifiedCursor::emitPositionChangedSignals()
{
    qDebug() << "[UnifiedCursor::emitPositionChangedSignals] 开始";
    
    CursorPosition cursorPos = CursorPositionAdapter::unifiedToCursor(m_position);
    
    qDebug() << "  m_position.isMathMode() = " << m_position.isMathMode();
    
    qDebug() << "  发出 positionChanged 信号";
    emit positionChanged(cursorPos);
    
    qDebug() << "  发出 unifiedPositionChanged 信号";
    emit unifiedPositionChanged(m_position);
}

// ========== 文档位置方法 ==========

/**
 * @brief 设置文档光标位置
 * @param blockIndex 块索引
 * @param offset 块内偏移量
 */
void UnifiedCursor::setDocumentPosition(int blockIndex, int offset)
{
    if (m_position.blockIndex != blockIndex || m_position.offset != offset) {
        m_position.blockIndex = blockIndex;
        m_position.offset = offset;
        m_position.mathPath = std::nullopt;
        m_position.mathTextOffset = 0;
        emitPositionChangedSignals();
    }
}

/**
 * @brief 向左移动光标
 */
void UnifiedCursor::moveLeft()
{
    if (!m_position.isDocumentMode()) {
        return;
    }
    
    // 在同一块内移动
    if (m_position.offset > 0) {
        m_position.offset--;
        emitPositionChangedSignals();
    } else if (m_position.blockIndex > 0) {
        // 移动到前一个块的末尾
        Block *prevBlock = m_document->block(m_position.blockIndex - 1);
        if (prevBlock) {
            m_position.blockIndex--;
            m_position.offset = prevBlock->length();
            emitPositionChangedSignals();
        }
    }
}

/**
 * @brief 向右移动光标
 */
void UnifiedCursor::moveRight()
{
    if (!m_position.isDocumentMode()) {
        return;
    }
    
    Block *block = m_document->block(m_position.blockIndex);
    if (!block)
        return;
    
    if (m_position.offset < block->length()) {
        m_position.offset++;
        emitPositionChangedSignals();
    } else if (m_position.blockIndex < m_document->blockCount() - 1) {
        // 移动到下一个块的开头
        m_position.blockIndex++;
        m_position.offset = 0;
        emitPositionChangedSignals();
    }
}

/**
 * @brief 向上移动光标
 */
void UnifiedCursor::moveUp()
{
    if (!m_position.isDocumentMode()) {
        return;
    }
    
    // 移动到上一个块
    if (m_position.blockIndex > 0) {
        m_position.blockIndex--;
        emitPositionChangedSignals();
    }
}

/**
 * @brief 向下移动光标
 */
void UnifiedCursor::moveDown()
{
    if (!m_position.isDocumentMode()) {
        return;
    }
    
    if (m_position.blockIndex < m_document->blockCount() - 1) {
        m_position.blockIndex++;
        emitPositionChangedSignals();
    }
}

/**
 * @brief 移动到行首
 */
void UnifiedCursor::moveToStartOfLine()
{
    if (!m_position.isDocumentMode()) {
        return;
    }
    
    m_position.offset = 0;
    emitPositionChangedSignals();
}

/**
 * @brief 移动到行尾
 */
void UnifiedCursor::moveToEndOfLine()
{
    if (!m_position.isDocumentMode()) {
        return;
    }
    
    Block *block = m_document->block(m_position.blockIndex);
    if (block) {
        m_position.offset = block->length();
        emitPositionChangedSignals();
    }
}

/**
 * @brief 移动到文档开头
 */
void UnifiedCursor::moveToStartOfDocument()
{
    if (!m_position.isDocumentMode()) {
        return;
    }
    
    m_position.blockIndex = 0;
    m_position.offset = 0;
    emitPositionChangedSignals();
}

/**
 * @brief 移动到文档结尾
 */
void UnifiedCursor::moveToEndOfDocument()
{
    if (!m_position.isDocumentMode()) {
        return;
    }
    
    int lastBlock = m_document->blockCount() - 1;
    if (lastBlock >= 0) {
        Block *block = m_document->block(lastBlock);
        m_position.blockIndex = lastBlock;
        m_position.offset = block ? block->length() : 0;
        emitPositionChangedSignals();
    }
}

/**
 * @brief 在光标位置插入文本
 * @param text 要插入的文本
 * @param style 文本的字符样式
 */
void UnifiedCursor::insertText(const QString &text, const CharacterStyle &style)
{
    if (!m_position.isDocumentMode()) {
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
        emitPositionChangedSignals();
    }
}

/**
 * @brief 删除光标前一个字符
 */
void UnifiedCursor::deletePreviousChar()
{
    if (!m_position.isDocumentMode()) {
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
        emitPositionChangedSignals();
    }
}

/**
 * @brief 删除光标后一个字符
 */
void UnifiedCursor::deleteNextChar()
{
    if (!m_position.isDocumentMode()) {
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

// ========== 公式位置方法 ==========

/**
 * @brief 设置公式光标位置
 * @param mathPath 公式坐标路径
 */
void UnifiedCursor::setMathPosition(const CoordinatePath &mathPath)
{
    if (mathPath.isValid()) {
        m_position.mathPath = mathPath;
        m_position.mathTextOffset = 0;
        emitPositionChangedSignals();
    }
}

/**
 * @brief 在公式中向左移动光标
 */
void UnifiedCursor::mathMoveLeft()
{
    if (!m_position.isMathMode() || !m_position.mathPath->isValid()) {
        return;
    }
    
    // 直接减少 mathTextOffset（简单实现，适用于 GenericMathItem）
    if (m_position.mathTextOffset > 0) {
        m_position.mathTextOffset--;
        emitPositionChangedSignals();
    }
}

/**
 * @brief 在公式中向右移动光标
 */
void UnifiedCursor::mathMoveRight()
{
    if (!m_position.isMathMode() || !m_position.mathPath->isValid()) {
        return;
    }
    
    // 直接增加 mathTextOffset（简单实现，适用于 GenericMathItem）
    m_position.mathTextOffset++;
    emitPositionChangedSignals();
}

/**
 * @brief 在公式中向上移动光标
 */
void UnifiedCursor::mathMoveUp()
{
    if (!m_position.isMathMode() || !m_position.mathPath->isValid() || m_position.mathPath->isEmpty()) {
        return;
    }
    
    CoordinatePath &path = *m_position.mathPath;
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
                emitPositionChangedSignals();
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
    if (!m_position.isMathMode() || !m_position.mathPath->isValid() || m_position.mathPath->isEmpty()) {
        return;
    }
    
    CoordinatePath &path = *m_position.mathPath;
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
                emitPositionChangedSignals();
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
    if (!m_position.isMathMode() || !m_position.mathPath->isValid() || m_position.mathPath->depth() <= 1) {
        return;
    }
    
    // 弹出最后一段路径，移动到父容器
    m_position.mathPath->pop();
    emitPositionChangedSignals();
}

// ========== 退出公式模式 ==========

/**
 * @brief 退出公式模式，回到文档模式
 */
void UnifiedCursor::exitMathMode()
{
    if (m_position.isMathMode()) {
        m_position.mathPath = std::nullopt;
        m_position.mathTextOffset = 0;
        emitPositionChangedSignals();
    }
}

// ========== 公式内文本编辑方法（待完善） ==========

void UnifiedCursor::mathInsertText(const QString &text, const CharacterStyle &style)
{
    Q_UNUSED(text);
    Q_UNUSED(style);
    // TODO: 实现公式内文本插入功能
    // 需要根据 mathPath 找到对应的 GenericMathSpan 并修改文本
    qDebug() << "[UnifiedCursor::mathInsertText] 待实现: 插入文本到公式";
}

void UnifiedCursor::mathDeletePreviousChar()
{
    // 简单实现：如果 mathTextOffset > 0，就减少它
    if (m_position.isMathMode() && m_position.mathTextOffset > 0) {
        m_position.mathTextOffset--;
        emitPositionChangedSignals();
        qDebug() << "[UnifiedCursor::mathDeletePreviousChar] mathTextOffset 减少为:" << m_position.mathTextOffset;
    }
    // TODO: 真正删除公式内的字符需要找到 GenericMathSpan 并修改数据
}

void UnifiedCursor::mathDeleteNextChar()
{
    // 简单实现：增加 mathTextOffset（假装删除了后面的字符）
    if (m_position.isMathMode()) {
        m_position.mathTextOffset++;
        emitPositionChangedSignals();
        qDebug() << "[UnifiedCursor::mathDeleteNextChar] mathTextOffset 增加为:" << m_position.mathTextOffset;
    }
    // TODO: 真正删除公式内的字符需要找到 GenericMathSpan 并修改数据
}

} // namespace QtWordEditor

