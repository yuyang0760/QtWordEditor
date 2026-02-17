#ifndef REMOVEBLOCKCOMMAND_H
#define REMOVEBLOCKCOMMAND_H

#include "EditCommand.h"
#include <QPointer>
#include "core/document/Block.h"

namespace QtWordEditor {

/**
 * @brief The RemoveBlockCommand class removes a block from the document.
 */
class RemoveBlockCommand : public EditCommand
{
public:
    RemoveBlockCommand(Document *document, int index);
    ~RemoveBlockCommand() override;

    void redo() override;
    void undo() override;

private:
    int m_index;
    QPointer<Block> m_removedBlock;
};

} // namespace QtWordEditor

#endif // REMOVEBLOCKCOMMAND_H