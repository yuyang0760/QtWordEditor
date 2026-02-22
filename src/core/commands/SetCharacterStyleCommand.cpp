#include "core/commands/SetCharacterStyleCommand.h"
#include "core/document/Document.h"
#include "core/document/ParagraphBlock.h"
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
    // 清理旧的 spans
    qDeleteAll(m_oldSpans);
    m_oldSpans.clear();
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

    // 清理旧的 spans（如果有）
    qDeleteAll(m_oldSpans);
    m_oldSpans.clear();
    
    // 保存旧的 spans 用于撤销
    for (int i = 0; i < para->inlineSpanCount(); ++i) {
        InlineSpan *span = para->inlineSpan(i);
        m_oldSpans.append(span->clone());
    }

    // 使用 ParagraphBlock 的 setStyle 方法正确地应用样式到指定范围
    para->setStyle(m_start, m_end - m_start, m_newStyle);
}

void SetCharacterStyleCommand::undo()
{
    Block *block = document()->block(m_blockIndex);
    if (!block)
        return;
    ParagraphBlock *para = qobject_cast<ParagraphBlock*>(block);
    if (!para)
        return;

    // 恢复旧的 spans
    para->clearInlineSpans();
    for (InlineSpan *span : m_oldSpans) {
        para->addInlineSpan(span->clone());
    }
}

} // namespace QtWordEditor