/**
 * @file ParagraphCommand.cpp
 * @brief ParagraphCommand类的实现
 *
 * 本文件实现了ParagraphCommand类，提供通用的段落块获取和类型检查功能。
 */

#include "core/commands/ParagraphCommand.h"
#include "core/document/Document.h"
#include "core/document/ParagraphBlock.h"
#include <QDebug>

namespace QtWordEditor {

ParagraphCommand::ParagraphCommand(Document *document, int blockIndex, const QString &text)
    : EditCommand(document, text)
    , m_blockIndex(blockIndex)
{
}

ParagraphCommand::~ParagraphCommand()
{
}

ParagraphBlock *ParagraphCommand::getParagraphBlock() const
{
    Block *block = document()->block(m_blockIndex);
    if (!block) {
        qWarning() << "Block not found at index" << m_blockIndex;
        return nullptr;
    }
    
    ParagraphBlock *para = qobject_cast<ParagraphBlock*>(block);
    if (!para) {
        qWarning() << "Block is not a paragraph block";
        return nullptr;
    }
    
    return para;
}

int ParagraphCommand::blockIndex() const
{
    return m_blockIndex;
}

} // namespace QtWordEditor
