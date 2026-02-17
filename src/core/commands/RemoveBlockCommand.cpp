#include "core/commands/RemoveBlockCommand.h"
#include "core/document/Document.h"
#include "core/document/Section.h"
#include <QDebug>

namespace QtWordEditor {

RemoveBlockCommand::RemoveBlockCommand(Document *document, int index)
    : EditCommand(document, QString())
    , m_index(index)
{
    setText(QObject::tr("Remove block"));
}

RemoveBlockCommand::~RemoveBlockCommand()
{
    if (m_removedBlock)
        delete m_removedBlock;
}

void RemoveBlockCommand::redo()
{
    // Simplified: assume single section
    if (document()->sectionCount() == 0)
        return;
    Section *section = document()->section(0);
    if (m_index < 0 || m_index >= section->blockCount())
        return;
    m_removedBlock = section->block(m_index);
    section->removeBlock(m_index);
}

void RemoveBlockCommand::undo()
{
    if (!m_removedBlock)
        return;
    Section *section = document()->section(0);
    if (!section)
        return;
    section->insertBlock(m_index, m_removedBlock);
    m_removedBlock = nullptr; // ownership transferred back to section
}

} // namespace QtWordEditor