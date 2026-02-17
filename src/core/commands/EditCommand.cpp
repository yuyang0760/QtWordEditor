#include "core/commands/EditCommand.h"
#include "core/document/Document.h"


namespace QtWordEditor {

EditCommand::EditCommand(Document *document, const QString &text)
    : QUndoCommand(text)
    , m_document(document)
{
}

EditCommand::~EditCommand()
{
}

Document *EditCommand::document() const
{
    return m_document;
}

void EditCommand::undo()
{
}

void EditCommand::redo()
{
}

} // namespace QtWordEditor
