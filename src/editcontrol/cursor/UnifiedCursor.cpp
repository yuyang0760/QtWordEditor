/**
 * @file UnifiedCursor.cpp
 * @brief 统一光标类实现（无模式版本）
 * 
 * 完全采用坐标路径系统，不再区分模式
 */

#include "editcontrol/cursor/UnifiedCursor.h"
#include "core/document/Document.h"
#include "core/document/Section.h"
#include "core/document/Block.h"
#include "core/document/ParagraphBlock.h"
#include "core/document/math/GenericMathSpan.h"
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
 * @brief 在内部位置变化时，发出位置变化信号
 */
void UnifiedCursor::emitPositionChangedSignals()
{
    qDebug() << "[UnifiedCursor::emitPositionChangedSignals] 开始";
    
    qDebug() << "  m_position.isMathMode() = " << m_position.isMathMode();
    
    // 发出统一位置变化信号
    emit unifiedPositionChanged(m_position);
    
    // 同时发出文档模式或公式模式的信号（保持向后兼容）
    // if (m_position.isDocumentMode()) {
    //     qDebug() << "  发出 documentPositionChanged 信号";
    //     emit documentPositionChanged(m_position.blockIndex, m_position.offset);
    // } else if (m_position.isMathMode()) {
    //     qDebug() << "  发出 mathPositionChanged 信号";
    //     emit mathPositionChanged(m_position.blockIndex, m_position.offset, m_position.mathPath.value(), m_position.mathTextOffset);
    // }
}

// ========== 文档位置方法 ==========

/**
 * @brief 设置文档光标位置
 * @param blockIndex 块索引
 * @param offset 块内偏移量
 */
void UnifiedCursor::setDocumentPosition(int blockIndex, int offset)
{
    // 实现设置文档光标位置的逻辑
}

/**
 * @brief 向左移动光标
 */
void UnifiedCursor::moveLeft()
{
    // 实现向左移动光标的逻辑
}

/**
 * @brief 向右移动光标
 */
void UnifiedCursor::moveRight()
{
    // 实现向右移动光标的逻辑
}

/**
 * @brief 向上移动光标
 */
void UnifiedCursor::moveUp()
{
    // 实现向上移动光标的逻辑
}

/**
 * @brief 向下移动光标
 */
void UnifiedCursor::moveDown()
{
    // 实现向下移动光标的逻辑
}

/**
 * @brief 移动到行首
 */
void UnifiedCursor::moveToStartOfLine()
{
    // 实现移动到行首的逻辑
}

/**
 * @brief 移动到行尾
 */
void UnifiedCursor::moveToEndOfLine()
{
    // 实现移动到行尾的逻辑
}

/**
 * @brief 移动到文档开头
 */
void UnifiedCursor::moveToStartOfDocument()
{
    // 实现移动到文档开头的逻辑
}

/**
 * @brief 移动到文档结尾
 */
void UnifiedCursor::moveToEndOfDocument()
{
    // 实现移动到文档结尾的逻辑
}

/**
 * @brief 在光标位置插入文本
 * @param text 要插入的文本
 * @param style 文本的字符样式
 */
void UnifiedCursor::insertText(const QString &text, const CharacterStyle &style)
{
    qDebug() << "[UnifiedCursor::insertText] 开始，文本：" << text;
    
    if (text.isEmpty() || !m_document) {
        qDebug() << "[UnifiedCursor::insertText] 无效参数";
        return;
    }
    
    if (m_position.isDocumentMode()) {
        // 文档模式：在文档中插入文本
        qDebug() << "[UnifiedCursor::insertText] 文档模式";
        
        Block *block = m_document->block(m_position.blockIndex);
        if (block) {
            ParagraphBlock *paraBlock = dynamic_cast<ParagraphBlock*>(block);
            if (paraBlock) {
                // paraBlock->insertTextAtPosition(m_position.offset, text);
                
                // 更新光标位置
                m_position.offset += text.length();
                emitPositionChangedSignals();
            }
        }
    } else if (m_position.isMathMode()) {
        // 公式模式：在公式中插入文本
        qDebug() << "[UnifiedCursor::insertText] 公式模式";
        
        // 找到对应的 GenericMathSpan
        // GenericMathSpan *genericSpan = findGenericMathSpanFromPath(m_document, m_position);
        // if (genericSpan) {
        //     genericSpan->insertTextAtPosition(m_position.mathTextOffset, text);
        //     
        //     // 更新光标位置
        //     m_position.mathTextOffset += text.length();
        //     emitPositionChangedSignals();
        // }
    }
}

/**
 * @brief 删除光标前一个字符
 */
void UnifiedCursor::deletePreviousChar()
{
    qDebug() << "[UnifiedCursor::deletePreviousChar] 开始";
    
    if (!m_document) {
        qDebug() << "[UnifiedCursor::deletePreviousChar] 文档无效";
        return;
    }
    
    if (m_position.isDocumentMode()) {
        // 文档模式：删除文档中的前一个字符
        qDebug() << "[UnifiedCursor::deletePreviousChar] 文档模式";
        
        if (m_position.offset <= 0) {
            qDebug() << "[UnifiedCursor::deletePreviousChar] 光标已在开头";
            return;
        }
        
        Block *block = m_document->block(m_position.blockIndex);
        if (block) {
            ParagraphBlock *paraBlock = dynamic_cast<ParagraphBlock*>(block);
            if (paraBlock) {
                paraBlock->remove(m_position.offset - 1, 1);
                
                // 更新光标位置
                m_position.offset--;
                emitPositionChangedSignals();
            }
        }
    } else if (m_position.isMathMode()) {
        // 公式模式：删除公式中的前一个字符
        qDebug() << "[UnifiedCursor::deletePreviousChar] 公式模式";
        
        if (m_position.mathTextOffset <= 0) {
            qDebug() << "[UnifiedCursor::deletePreviousChar] 光标已在开头";
            return;
        }
        
        // 找到对应的 GenericMathSpan
        // GenericMathSpan *genericSpan = findGenericMathSpanFromPath(m_document, m_position);
        // if (genericSpan) {
        //     genericSpan->remove(m_position.mathTextOffset - 1, 1);
        //     
        //     // 更新光标位置
        //     m_position.mathTextOffset--;
        //     emitPositionChangedSignals();
        // }
    }
}

/**
 * @brief 删除光标后一个字符
 */
void UnifiedCursor::deleteNextChar()
{
    qDebug() << "[UnifiedCursor::deleteNextChar] 开始";
    
    if (!m_document) {
        qDebug() << "[UnifiedCursor::deleteNextChar] 文档无效";
        return;
    }
    
    if (m_position.isDocumentMode()) {
        // 文档模式：删除文档中的后一个字符
        qDebug() << "[UnifiedCursor::deleteNextChar] 文档模式";
        
        Block *block = m_document->block(m_position.blockIndex);
        if (block) {
            ParagraphBlock *paraBlock = dynamic_cast<ParagraphBlock*>(block);
            if (paraBlock) {
                if (m_position.offset < paraBlock->length()) {
                    paraBlock->remove(m_position.offset, 1);
                    
                    // 光标位置不需要改变
                    emitPositionChangedSignals();
                }
            }
        }
    } else if (m_position.isMathMode()) {
        // 公式模式：删除公式中的后一个字符
        qDebug() << "[UnifiedCursor::deleteNextChar] 公式模式";
        
        // 找到对应的 GenericMathSpan
        // GenericMathSpan *genericSpan = findGenericMathSpanFromPath(m_document, m_position);
        // if (genericSpan) {
        //     if (m_position.mathTextOffset < genericSpan->length()) {
        //         genericSpan->remove(m_position.mathTextOffset, 1);
        //         
        //         // 光标位置不需要改变
        //         emitPositionChangedSignals();
        //     }
        // }
    }
}

// ========== 公式位置方法 ==========

/**
 * @brief 设置公式光标位置
 * @param mathPath 公式坐标路径
 */
void UnifiedCursor::setMathPosition(const CoordinatePath &mathPath)
{
    // 实现设置公式光标位置的逻辑
}

/**
 * @brief 在公式中向左移动光标
 */
void UnifiedCursor::mathMoveLeft()
{
    // 实现在公式中向左移动光标的逻辑
}

/**
 * @brief 在公式中向右移动光标
 */
void UnifiedCursor::mathMoveRight()
{
    // 实现在公式中向右移动光标的逻辑
}

/**
 * @brief 在公式中向上移动光标
 */
void UnifiedCursor::mathMoveUp()
{
    // 实现在公式中向上移动光标的逻辑
}

/**
 * @brief 在公式中向下移动光标
 */
void UnifiedCursor::mathMoveDown()
{
    // 实现在公式中向下移动光标的逻辑
}

/**
 * @brief 移动到公式父容器
 */
void UnifiedCursor::mathMoveToParent()
{
    // 实现移动到公式父容器的逻辑
}

// ========== 退出公式模式 ==========

/**
 * @brief 退出公式模式，回到文档模式
 */
void UnifiedCursor::exitMathMode()
{
    // 实现退出公式模式的逻辑
}

// ========== 公式内文本编辑 ==========

/**
 * @brief 在公式内光标位置插入文本
 * @param text 要插入的文本
 * @param style 文本的字符样式
 */
void UnifiedCursor::mathInsertText(const QString &text, const CharacterStyle &style)
{
    // 实现在公式内光标位置插入文本的逻辑
}

/**
 * @brief 删除公式内光标前一个字符
 */
void UnifiedCursor::mathDeletePreviousChar()
{
    // if (!m_position.isMathMode() || m_position.mathTextOffset <= 0 || !m_document) {
    //     return;
    // }
    // 
    // qDebug() << "[UnifiedCursor::mathDeletePreviousChar] 开始, mathTextOffset=" << m_position.mathTextOffset;
    // 
    // // 找到对应的 GenericMathSpan
    // GenericMathSpan *genericSpan = findGenericMathSpanFromPath(m_document, m_position);
    // if (!genericSpan) {
    //     qDebug() << "[UnifiedCursor::mathDeletePreviousChar] 找不到 GenericMathSpan";
    //     return;
    // }
    // 
    // qDebug() << "[UnifiedCursor::mathDeletePreviousChar] 找到 GenericMathSpan, 删除前一个字符";
    // 
    // // 删除前一个字符
    // genericSpan->remove(m_position.mathTextOffset - 1, 1);
    // 
    // // 更新光标位置
    // m_position.mathTextOffset--;
    // emitPositionChangedSignals();
}

/**
 * @brief 删除公式内光标后一个字符
 */
void UnifiedCursor::mathDeleteNextChar()
{
    // if (!m_position.isMathMode() || !m_document) {
    //     return;
    // }
    // 
    // qDebug() << "[UnifiedCursor::mathDeleteNextChar] 开始, mathTextOffset=" << m_position.mathTextOffset;
    // 
    // // 找到对应的 GenericMathSpan
    // GenericMathSpan *genericSpan = findGenericMathSpanFromPath(m_document, m_position);
    // if (!genericSpan) {
    //     qDebug() << "[UnifiedCursor::mathDeleteNextChar] 找不到 GenericMathSpan";
    //     return;
    // }
    // 
    // qDebug() << "[UnifiedCursor::mathDeleteNextChar] 找到 GenericMathSpan, 删除后一个字符";
    // 
    // // 删除后一个字符
    // genericSpan->remove(m_position.mathTextOffset, 1);
    // 
    // // 光标位置不需要改变
    // emitPositionChangedSignals();
}

/**
 * @brief 获取统一光标位置的字符样式
 * @return 当前位置的字符样式
 */
CharacterStyle UnifiedCursor::styleAtUnifiedPosition() const
{
    if (m_position.isDocumentMode()) {
        // 文档模式：获取文档位置的字符样式
        Block *block = m_document->block(m_position.blockIndex);
        if (block) {
            // 使用 dynamic_cast 检查 block 是否是 ParagraphBlock 类型
            ParagraphBlock *paraBlock = dynamic_cast<ParagraphBlock*>(block);
            if (paraBlock) {
                return paraBlock->styleAt(m_position.offset);
            }
        }
    } else if (m_position.isMathMode()) {
        // 公式模式：获取公式位置的字符样式
        // GenericMathSpan *genericSpan = findGenericMathSpanFromPath(m_document, m_position);
        // if (genericSpan) {
        //     return genericSpan->styleAt(m_position.mathTextOffset);
        // }
    }
    
    // 默认返回空样式
    return CharacterStyle();
}

} // namespace QtWordEditor