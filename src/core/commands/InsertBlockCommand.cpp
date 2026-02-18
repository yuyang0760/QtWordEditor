
/**
 * @file InsertBlockCommand.cpp
 * @brief InsertBlockCommand类的实现
 *
 * 本文件实现了InsertBlockCommand类，用于在文档中插入新的块，
 * 支持撤销和重做操作。
 */

#include "core/commands/InsertBlockCommand.h"
#include "core/document/Document.h"
#include "core/document/Section.h"
#include <QDebug>

namespace QtWordEditor {

/**
 * @brief 构造InsertBlockCommand对象
 * @param document 要操作的文档
 * @param index 插入位置的全局索引
 * @param block 要插入的块对象
 */
InsertBlockCommand::InsertBlockCommand(Document *document, int index, Block *block)
    : EditCommand(document, QString())
    , m_index(index)
    , m_block(block)
{
    setText(QObject::tr("Insert block"));
}

/**
 * @brief 销毁InsertBlockCommand对象
 *
 * 注意：该类拥有m_block的所有权，会在析构时删除它
 */
InsertBlockCommand::~InsertBlockCommand()
{
    if (m_block)
        delete m_block;
}

/**
 * @brief 执行插入块操作（重做）
 *
 * 将块插入到文档中。如果文档没有章节，会先创建一个默认章节。
 */
void InsertBlockCommand::redo()
{
    if (!m_block) {
        qWarning() << "Block is null";
        return;
    }
    if (document()->sectionCount() == 0) {
        Section *sec = new Section(document());
        document()->addSection(sec);
    }
    Section *section = document()->section(0);
    section->addBlock(m_block);
    m_block->setParent(section);
}

/**
 * @brief 撤销插入块操作
 *
 * 从文档中移除之前插入的块
 */
void InsertBlockCommand::undo()
{
    if (!m_block)
        return;
    Section *section = document()->section(0);
    if (!section)
        return;
    for (int i = 0; i < section->blockCount(); ++i) {
        if (section->block(i) == m_block) {
            section->removeBlock(i);
            break;
        }
    }
}

} // namespace QtWordEditor

