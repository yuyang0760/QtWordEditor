
/**
 * @file RemoveTextCommand.cpp
 * @brief RemoveTextCommand类的实现
 *
 * 本文件实现了RemoveTextCommand类，用于从段落块中移除文本，
 * 支持撤销和重做操作。
 */

#include "core/commands/RemoveTextCommand.h"
#include "core/document/Document.h"
#include "core/document/ParagraphBlock.h"
#include <QDebug>

namespace QtWordEditor {

/**
 * @brief 构造RemoveTextCommand对象
 * @param document 要操作的文档
 * @param blockIndex 要移除文本的块的全局索引
 * @param position 在块中的起始位置
 * @param length 要移除的字符数
 */
RemoveTextCommand::RemoveTextCommand(Document *document, int blockIndex, int position, int length)
    : EditCommand(document, QString())
    , m_blockIndex(blockIndex)
    , m_position(position)
    , m_length(length)
    , m_oldBlock(nullptr)
{
    setText(QObject::tr("Delete text"));
}

/**
 * @brief 销毁RemoveTextCommand对象
 */
RemoveTextCommand::~RemoveTextCommand()
{
    if (m_oldBlock) {
        delete m_oldBlock;
    }
}

/**
 * @brief 执行移除文本操作（重做）
 *
 * 从段落块中移除指定范围的文本，并保存被移除的文本以便撤销
 */
void RemoveTextCommand::redo()
{
    Block *block = document()->block(m_blockIndex);
    if (!block) {
        qWarning() << "Block not found at index" << m_blockIndex;
        return;
    }
    ParagraphBlock *para = qobject_cast<ParagraphBlock*>(block);
    if (!para) {
        qWarning() << "Block is not a paragraph block";
        return;
    }
    
    // 保存旧的块用于撤销
    if (m_oldBlock) {
        delete m_oldBlock;
    }
    m_oldBlock = qobject_cast<ParagraphBlock*>(para->clone());
    
    // 执行删除操作
    para->remove(m_position, m_length);
}

/**
 * @brief 撤销移除文本操作
 *
 * 将之前移除的文本重新插入到段落块的原位置
 */
void RemoveTextCommand::undo()
{
    if (!m_oldBlock) {
        return;
    }
    
    Block *block = document()->block(m_blockIndex);
    if (!block)
        return;
    ParagraphBlock *para = qobject_cast<ParagraphBlock*>(block);
    if (!para)
        return;
    
    // 恢复旧的块
    para->clearInlineSpans();
    for (int i = 0; i < m_oldBlock->inlineSpanCount(); ++i) {
        InlineSpan *span = m_oldBlock->inlineSpan(i);
        para->addInlineSpan(span->clone());
    }
    para->setParagraphStyle(m_oldBlock->paragraphStyle());
}

} // namespace QtWordEditor

