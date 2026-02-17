#include "include/core/commands/SetCharacterStyleCommand.h"
#include "include/core/document/Document.h"
#include "include/core/document/ParagraphBlock.h"
#include <QDebug>

namespace QtWordEditor {

SetCharacterStyleCommand::SetCharacterStyleCommand(Document *document, int blockIndex,
                                                   int start, int end,
                                                   const CharacterStyle &style)
    : EditCommand(document, QString())
    , m_blockIndex(blockIndex)
    , m_start(start)
    , m_end(end)
    , m_newStyle(style)
{
    setText(QObject::tr("Change character style"));
}

SetCharacterStyleCommand::~SetCharacterStyleCommand()
{
}

void SetCharacterStyleCommand::redo()
{
    Block *block = document()->block(m_blockIndex);
    if (!block) {
        qWarning() << "Block not found at index" << m_blockIndex;
        return;
    }
    ParagraphBlock *para = qobject_cast<ParagraphBlock*>(block);
    if (!para) {
        qWarning() << "Block is not a paragraph block";
        return;
    }

    // Save old spans for undo
    m_oldSpans.clear();
    for (int i = 0; i < para->spanCount(); ++i) {
        m_oldSpans.append(para->span(i));
    }

    // Apply new style to the range (simplified: assume single span)
    // TODO: implement proper style application across multiple spans
    // For now, just set the style of the whole block (placeholder)
    for (int i = 0; i < para->spanCount(); ++i) {
        Span span = para->span(i);
        span.setStyle(m_newStyle);
        para->setSpan(i, span);
    }
}

void SetCharacterStyleCommand::undo()
{
    Block *block = document()->block(m_blockIndex);
    if (!block)
        return;
    ParagraphBlock *para = qobject_cast<ParagraphBlock*>(block);
    if (!para)
        return;

    // Restore old spans
    para->setText(""); // clear
    for (const Span &span : m_oldSpans) {
        para->addSpan(span);
    }
}

} // namespace QtWordEditor