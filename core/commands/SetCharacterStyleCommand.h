#ifndef SETCHARACTERSTYLECOMMAND_H
#define SETCHARACTERSTYLECOMMAND_H

#include "EditCommand.h"
#include "core/document/CharacterStyle.h"
#include <QList>
#include "core/document/Span.h"

namespace QtWordEditor {

/**
 * @brief The SetCharacterStyleCommand applies a character style to a selection.
 */
class SetCharacterStyleCommand : public EditCommand
{
public:
    SetCharacterStyleCommand(Document *document, int blockIndex, int start, int end,
                             const CharacterStyle &style);
    ~SetCharacterStyleCommand() override;

    void redo() override;
    void undo() override;

private:
    int m_blockIndex;
    int m_start;
    int m_end;
    CharacterStyle m_newStyle;
    QList<Span> m_oldSpans; // original spans for undo
};

} // namespace QtWordEditor

#endif // SETCHARACTERSTYLECOMMAND_H