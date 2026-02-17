#include "InsertBlockCommand.h"
#include "core/document/Document.h"
#include "core/document/Section.h"
#include <QDebug>

namespace QtWordEditor {

InsertBlockCommand::InsertBlockCommand(Document *document, int index, Block *block)
    : EditCommand(document, QString())
    , m_index(index)
    , m_block(block)
{
    setText(QObject::tr("Insert block"));
}

InsertBlockCommand::~InsertBlockCommand()
{
    if (m_block)
        delete m_block;
}

void InsertBlockCommand::redo()
{
    if (!m_block) {
        qWarning() << "Block is null";
        return;
    }
    // Simplified: assume document has at least one section
    if (document()->sectionCount() == 0) {
        // Create a default section
        Section *sec = new Section(document());
        document()->addSection(sec);
    }
    Section *section = document()->section(0);
    // Convert global index to section local index (simplified)
    // For now, just insert at the end of the section
    section->addBlock(m_block);
    m_block->setParent(section);
    // TODO: update global index mapping
}

void InsertBlockCommand::undo()
{
    if (!m_block)
        return;
    Section *section = document()->section(0);
    if (!section)
        return;
    // Find the block in the section and remove it
    for (int i = 0; i < section->blockCount(); ++i) {
        if (section->block(i) == m_block) {
            section->removeBlock(i);
            break;
        }
    }
}

} // namespace QtWordEditor