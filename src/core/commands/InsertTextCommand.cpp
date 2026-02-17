#include "include/core/commands/InsertTextCommand.h"
#include "include/core/document/Document.h"
#include "include/core/document/ParagraphBlock.h"
#include <QDebug>

namespace QtWordEditor {

InsertTextCommand::InsertTextCommand(Document *document, int blockIndex, int position,
                                     const QString &text, const CharacterStyle &style)
    : EditCommand(document, QString())
    , m_blockIndex(blockIndex)
    , m_position(position)
    , m_text(text)
    , m_style(style)
{
    setText(QObject::tr("Insert text"));
}

InsertTextCommand::~InsertTextCommand()
{
}

void InsertTextCommand::redo()
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
    para->insert(m_position, m_text, m_style);
}

void InsertTextCommand::undo()
{
    Block *block = document()->block(m_blockIndex);
    if (!block)
        return;
    ParagraphBlock *para = qobject_cast<ParagraphBlock*>(block);
    if (!para)
        return;
    // Remove the inserted text
    para->remove(m_position, m_text.length());
}

bool InsertTextCommand::mergeWith(const QUndoCommand *other)
{
    const InsertTextCommand *cmd = dynamic_cast<const InsertTextCommand*>(other);
    if (!cmd)
        return false;
    // Merge if same block and contiguous insertion
    if (m_blockIndex == cmd->m_blockIndex &&
        m_position + m_text.length() == cmd->m_position &&
        m_style == cmd->m_style) {
        m_text += cmd->m_text;
        return true;
    }
    return false;
}

} // namespace QtWordEditor