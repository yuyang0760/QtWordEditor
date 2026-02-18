
/**
 * @file InsertTextCommand.cpp
 * @brief InsertTextCommand类的实现
 *
 * 本文件实现了InsertTextCommand类，用于在段落块中插入文本，
 * 支持撤销、重做以及与相邻的插入文本命令合并。
 */

#include "core/commands/InsertTextCommand.h"
#include "core/document/Document.h"
#include "core/document/ParagraphBlock.h"
#include &lt;QDebug&gt;

namespace QtWordEditor {

/**
 * @brief 构造InsertTextCommand对象
 * @param document 要操作的文档
 * @param blockIndex 要插入文本的块的全局索引
 * @param position 在块中的插入位置
 * @param text 要插入的文本内容
 * @param style 插入文本的字符样式
 */
InsertTextCommand::InsertTextCommand(Document *document, int blockIndex, int position,
                                     const QString &amp;text, const CharacterStyle &amp;style)
    : EditCommand(document, QString())
    , m_blockIndex(blockIndex)
    , m_position(position)
    , m_text(text)
    , m_style(style)
{
    setText(QObject::tr("Insert text"));
}

/**
 * @brief 销毁InsertTextCommand对象
 */
InsertTextCommand::~InsertTextCommand()
{
}

/**
 * @brief 执行插入文本操作（重做）
 *
 * 在指定的段落块的指定位置插入文本
 */
void InsertTextCommand::redo()
{
    Block *block = document()-&gt;block(m_blockIndex);
    if (!block) {
        qWarning() &lt;&lt; "Block not found at index" &lt;&lt; m_blockIndex;
        return;
    }
    ParagraphBlock *para = qobject_cast&lt;ParagraphBlock*&gt;(block);
    if (!para) {
        qWarning() &lt;&lt; "Block is not a paragraph block";
        return;
    }
    para-&gt;insert(m_position, m_text, m_style);
}

/**
 * @brief 撤销插入文本操作
 *
 * 从段落块中移除之前插入的文本
 */
void InsertTextCommand::undo()
{
    Block *block = document()-&gt;block(m_blockIndex);
    if (!block)
        return;
    ParagraphBlock *para = qobject_cast&lt;ParagraphBlock*&gt;(block);
    if (!para)
        return;
    para-&gt;remove(m_position, m_text.length());
}

/**
 * @brief 尝试与另一个命令合并
 * @param other 要合并的命令
 * @return 如果合并成功返回true，否则返回false
 *
 * 当连续插入文本且样式相同时，多个插入命令可以合并为一个，
 * 这样可以减少撤销栈中的命令数量。
 */
bool InsertTextCommand::mergeWith(const QUndoCommand *other)
{
    const InsertTextCommand *cmd = dynamic_cast&lt;const InsertTextCommand*&gt;(other);
    if (!cmd)
        return false;
    if (m_blockIndex == cmd-&gt;m_blockIndex &amp;&amp;
        m_position + m_text.length() == cmd-&gt;m_position &amp;&amp;
        m_style == cmd-&gt;m_style) {
        m_text += cmd-&gt;m_text;
        return true;
    }
    return false;
}

} // namespace QtWordEditor

