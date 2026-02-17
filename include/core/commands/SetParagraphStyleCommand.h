#ifndef SETPARAGRAPHSTYLECOMMAND_H
#define SETPARAGRAPHSTYLECOMMAND_H

#include "EditCommand.h"
#include "core/document/ParagraphStyle.h"
#include <QList>

namespace QtWordEditor {

/**
 * @brief The SetParagraphStyleCommand class applies paragraph styles to blocks.
 */
class SetParagraphStyleCommand : public EditCommand
{
public:
    SetParagraphStyleCommand(Document *document, const QList<int> &blockIndices,
                            const ParagraphStyle &newStyle);
    ~SetParagraphStyleCommand() override;

    void redo() override;
    void undo() override;

private:
    QList<int> m_blockIndices;
    ParagraphStyle m_newStyle;
    QList<ParagraphStyle> m_oldStyles;
};

} // namespace QtWordEditor

#endif // SETPARAGRAPHSTYLECOMMAND_H