
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
{
    setText(QObject::tr("Delete text"));
}

/**
 * @brief 销毁RemoveTextCommand对象
 */
RemoveTextCommand::~RemoveTextCommand()
{
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
    m_removedText = para->text().mid(m_position, m_length);
    para->remove(m_position, m_length);
}

/**
 * @brief 撤销移除文本操作
 *
 * 将之前移除的文本重新插入到段落块的原位置
 */
void RemoveTextCommand::undo()
{
    Block *block = document()->block(m_blockIndex);
    if (!block)
        return;
    ParagraphBlock *para = qobject_cast<ParagraphBlock*>(block);
    if (!para)
        return;
    for (const auto& span : m_removedSpans) {
        para->insert(m_position, span.text(), span.style());
        m_position += span.text().length();
    }
}

} // namespace QtWordEditor

