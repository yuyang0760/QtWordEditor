#ifndef REMOVETEXTCOMMAND_H
#define REMOVETEXTCOMMAND_H

#include "EditCommand.h"
#include "core/document/CharacterStyle.h"
#include "core/document/Span.h"
#include <QString>
#include <QList>

namespace QtWordEditor {

/**
 * @brief The RemoveTextCommand class removes a range of text from a block.
 */
class RemoveTextCommand : public EditCommand
{
public:
    RemoveTextCommand(Document *document, int blockIndex, int position, int length);
    ~RemoveTextCommand() override;

    void redo() override;
    void undo() override;

private:
    int m_blockIndex;
    int m_position;
    int m_length;
    QString m_removedText;
    QList<Span> m_removedSpans;
};

} // namespace QtWordEditor

#endif // REMOVETEXTCOMMAND_H