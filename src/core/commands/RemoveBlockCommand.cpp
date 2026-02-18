
/**
 * @file RemoveBlockCommand.cpp
 * @brief RemoveBlockCommand类的实现
 *
 * 本文件实现了RemoveBlockCommand类，用于从文档中移除块，
 * 支持撤销和重做操作。
 */

#include "core/commands/RemoveBlockCommand.h"
#include "core/document/Document.h"
#include "core/document/Section.h"
#include &lt;QDebug&gt;

namespace QtWordEditor {

/**
 * @brief 构造RemoveBlockCommand对象
 * @param document 要操作的文档
 * @param index 要移除的块的全局索引
 */
RemoveBlockCommand::RemoveBlockCommand(Document *document, int index)
    : EditCommand(document, QString())
    , m_index(index)
{
    setText(QObject::tr("Remove block"));
}

/**
 * @brief 销毁RemoveBlockCommand对象
 *
 * 注意：如果块还没有被重新插入，会在这里删除它
 */
RemoveBlockCommand::~RemoveBlockCommand()
{
    if (m_removedBlock)
        delete m_removedBlock;
}

/**
 * @brief 执行移除块操作（重做）
 *
 * 从文档的第一个章节中移除指定索引的块，并保存被移除的块以便撤销
 */
void RemoveBlockCommand::redo()
{
    if (document()-&gt;sectionCount() == 0)
        return;
    Section *section = document()-&gt;section(0);
    if (m_index &lt; 0 || m_index &gt;= section-&gt;blockCount())
        return;
    m_removedBlock = section-&gt;block(m_index);
    section-&gt;removeBlock(m_index);
}

/**
 * @brief 撤销移除块操作
 *
 * 将之前移除的块重新插入到文档的原位置
 */
void RemoveBlockCommand::undo()
{
    if (!m_removedBlock)
        return;
    Section *section = document()-&gt;section(0);
    if (!section)
        return;
    section-&gt;insertBlock(m_index, m_removedBlock);
    m_removedBlock = nullptr;
}

} // namespace QtWordEditor

