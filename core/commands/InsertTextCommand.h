#ifndef INSERTTEXTCOMMAND_H
#define INSERTTEXTCOMMAND_H

#include "EditCommand.h"
#include "core/document/CharacterStyle.h"
#include <QString>

namespace QtWordEditor {

/**
 * @brief The InsertTextCommand class inserts text at a given position within a block.
 */
class InsertTextCommand : public EditCommand
{
public:
    InsertTextCommand(Document *document, int blockIndex, int position,
                      const QString &text, const CharacterStyle &style);
    ~InsertTextCommand() override;

    void redo() override;
    void undo() override;

    bool mergeWith(const QUndoCommand *other) override;

private:
    int m_blockIndex;
    int m_position;
    QString m_text;
    CharacterStyle m_style;
};

} // namespace QtWordEditor

#endif // INSERTTEXTCOMMAND_H