#include "include/core/commands/RemoveTextCommand.h"
#include "include/core/document/Document.h"
#include "include/core/document/ParagraphBlock.h"
#include <QDebug>

namespace QtWordEditor {

RemoveTextCommand::RemoveTextCommand(Document *document, int blockIndex, int position, int length)
    : EditCommand(document, QString())
    , m_blockIndex(blockIndex)
    , m_position(position)
    , m_length(length)
{
    setText(QObject::tr("Delete text"));
}

RemoveTextCommand::~RemoveTextCommand()
{
}

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
    // Save the text to be removed (simplified: only plain text, no style)
    m_removedText = para->text().mid(m_position, m_length);
    // TODO: save the actual style(s) of the removed text.
    para->remove(m_position, m_length);
}

void RemoveTextCommand::undo()
{
    Block *block = document()->block(m_blockIndex);
    if (!block)
        return;
    ParagraphBlock *para = qobject_cast<ParagraphBlock*>(block);
    if (!para)
        return;
    // Re‑insert the saved text with the saved spans
    for (const auto& span : m_removedSpans) {
        para->insert(m_position, span.text(), span.style());
        m_position += span.text().length();
    }
}

} // namespace QtWordEditor