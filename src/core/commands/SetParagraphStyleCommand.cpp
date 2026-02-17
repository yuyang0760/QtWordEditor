#include "include/core/commands/SetParagraphStyleCommand.h"
#include "include/core/document/Document.h"
#include "include/core/document/Section.h"
#include "include/core/document/Block.h"
#include "include/core/document/ParagraphBlock.h"

namespace QtWordEditor {

SetParagraphStyleCommand::SetParagraphStyleCommand(Document *document,
                                                 const QList<int> &blockIndices,
                                                 const ParagraphStyle &newStyle)
    : EditCommand(document, "Set Paragraph Style")
    , m_blockIndices(blockIndices)
    , m_newStyle(newStyle)
{
    // Store old styles for undo
    for (int index : blockIndices) {
        Block *block = document->block(index);
        if (auto *paragraphBlock = dynamic_cast<ParagraphBlock*>(block)) {
            m_oldStyles.append(paragraphBlock->paragraphStyle());
        } else {
            m_oldStyles.append(ParagraphStyle()); // Default style
        }
    }
}

SetParagraphStyleCommand::~SetParagraphStyleCommand()
{
}

void SetParagraphStyleCommand::redo()
{
    Document *doc = document();
    for (int i = 0; i < m_blockIndices.size(); ++i) {
        int index = m_blockIndices.at(i);
        Block *block = doc->block(index);
        if (auto *paragraphBlock = dynamic_cast<ParagraphBlock*>(block)) {
            paragraphBlock->setParagraphStyle(m_newStyle);
        }
    }
}

void SetParagraphStyleCommand::undo()
{
    Document *doc = document();
    for (int i = 0; i < m_blockIndices.size(); ++i) {
        int index = m_blockIndices.at(i);
        Block *block = doc->block(index);
        if (auto *paragraphBlock = dynamic_cast<ParagraphBlock*>(block)) {
            paragraphBlock->setParagraphStyle(m_oldStyles.at(i));
        }
    }
}

} // namespace QtWordEditor