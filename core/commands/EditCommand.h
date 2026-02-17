#ifndef EDITCOMMAND_H
#define EDITCOMMAND_H

#include <QUndoCommand>
#include <QString>
#include "core/Global.h"

namespace QtWordEditor {

class Document;

/**
 * @brief The EditCommand class is the base class for all undoable editing commands.
 */
class EditCommand : public QUndoCommand
{
public:
    explicit EditCommand(Document *document, const QString &text = QString());
    ~EditCommand() override;

    Document *document() const;

    // Override QUndoCommand methods
    void undo() override;
    void redo() override;

protected:
    Document *m_document;

private:
    QString m_text;
};

} // namespace QtWordEditor

#endif // EDITCOMMAND_H