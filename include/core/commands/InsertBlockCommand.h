#ifndef INSERTBLOCKCOMMAND_H
#define INSERTBLOCKCOMMAND_H

#include "EditCommand.h"
#include <QPointer>
#include "core/document/Block.h"

namespace QtWordEditor {

/**
 * @brief The InsertBlockCommand class inserts a new block into the document.
 */
class InsertBlockCommand : public EditCommand
{
public:
    InsertBlockCommand(Document *document, int index, Block *block);
    ~InsertBlockCommand() override;

    void redo() override;
    void undo() override;

private:
    int m_index;
    QPointer<Block> m_block;
};

} // namespace QtWordEditor

#endif // INSERTBLOCKCOMMAND_H