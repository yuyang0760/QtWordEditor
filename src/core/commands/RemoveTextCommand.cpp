

/**
 * @file RemoveTextCommand.cpp
 * @brief RemoveTextCommand类的实现
 *
 * 本文件实现了RemoveTextCommand类，用于从段落块中移除文本，
 * 支持撤销和重做操作。
 */

#include "core/commands/RemoveTextCommand.h"
#include "core/document/Document.h"
#include "core/document/ParagraphBlock.h"
#include "core/document/TextSpan.h"
#include <QDebug>

namespace QtWordEditor {

/**
 * @brief 构造RemoveTextCommand对象
 * @param document 要操作的文档
 * @param blockIndex 要移除文本的块的全局索引
 * @param position 在块中的起始位置
 * @param length 要移除的字符数
 */
RemoveTextCommand::RemoveTextCommand(Document *document, int blockIndex, int position, int length)
    : ParagraphCommand(document, blockIndex, QObject::tr("Delete text"))
    , m_position(position)
    , m_length(length)
{
}

/**
 * @brief 销毁RemoveTextCommand对象
 */
RemoveTextCommand::~RemoveTextCommand()
{
    // 清理旧的 spans
    qDeleteAll(m_removedSpans);
    m_removedSpans.clear();
}

/**
 * @brief 执行移除文本操作（重做）
 *
 * 从段落块中移除指定范围的文本，并保存被移除的文本以便撤销
 */
void RemoveTextCommand::redo()
{
    ParagraphBlock *para = getParagraphBlock();
    if (!para)
        return;
    
    // 保存被删除的文本
    m_removedText = para->text().mid(m_position, m_length);
    
    // 保存被删除的 spans（深拷贝）
    qDeleteAll(m_removedSpans);
    m_removedSpans.clear();
    
    int end = m_position + m_length;
    int posInStartSpan = 0;
    int startSpanIndex = para->findInlineSpanIndex(m_position, &posInStartSpan);
    
    int posInEndSpan = 0;
    int endSpanIndex = para->findInlineSpanIndex(end, &posInEndSpan);
    
    // 保存被删除的 spans
    if (startSpanIndex == endSpanIndex) {
        InlineSpan *span = para->inlineSpan(startSpanIndex);
        if (span && span->type() == InlineSpan::Text) {
            const TextSpan *textSpan = static_cast<const TextSpan*>(span);
            QString removedText = textSpan->text().mid(posInStartSpan, m_length);
            m_removedSpans.append(new TextSpan(removedText, textSpan->style()));
        }
    } else {
        // 保存起始 span 的被删除部分
        if (startSpanIndex >= 0 && startSpanIndex < para->inlineSpanCount()) {
            InlineSpan *span = para->inlineSpan(startSpanIndex);
            if (span && span->type() == InlineSpan::Text) {
                const TextSpan *textSpan = static_cast<const TextSpan*>(span);
                QString removedText = textSpan->text().mid(posInStartSpan);
                m_removedSpans.append(new TextSpan(removedText, textSpan->style()));
            }
        }
        
        // 保存中间的 spans
        for (int i = startSpanIndex + 1; i < endSpanIndex; ++i) {
            if (i >= 0 && i < para->inlineSpanCount()) {
                InlineSpan *span = para->inlineSpan(i);
                if (span) {
                    m_removedSpans.append(span->clone());
                }
            }
        }
        
        // 保存结束 span 的被删除部分
        if (endSpanIndex >= 0 && endSpanIndex < para->inlineSpanCount()) {
            InlineSpan *span = para->inlineSpan(endSpanIndex);
            if (span && span->type() == InlineSpan::Text) {
                const TextSpan *textSpan = static_cast<const TextSpan*>(span);
                QString removedText = textSpan->text().left(posInEndSpan);
                m_removedSpans.append(new TextSpan(removedText, textSpan->style()));
            }
        }
    }
    
    // 删除文本
    para->remove(m_position, m_length);
}

/**
 * @brief 撤销移除文本操作
 *
 * 将之前移除的文本重新插入到段落块的原位置
 */
void RemoveTextCommand::undo()
{
    ParagraphBlock *para = getParagraphBlock();
    if (!para)
        return;
    
    // 恢复被删除的 spans
    int insertPos = m_position;
    for (const InlineSpan *span : m_removedSpans) {
        if (span->type() == InlineSpan::Text) {
            const TextSpan *textSpan = static_cast<const TextSpan*>(span);
            para->insert(insertPos, textSpan->text(), textSpan->style());
            insertPos += textSpan->length();
        }
    }
}

} // namespace QtWordEditor
