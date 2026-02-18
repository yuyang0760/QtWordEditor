#include "editcontrol/selection/Selection.h"
#include "core/document/Document.h"
#include "core/document/Block.h"
#include "core/document/ParagraphBlock.h"
#include <QDebug>

namespace QtWordEditor {

Selection::Selection(Document *document, QObject *parent)
    : QObject(parent)
    , m_document(document)
{
}

Selection::~Selection()
{
}

Document *Selection::document() const
{
    return m_document;
}

void Selection::setRange(const SelectionRange &range)
{
    m_ranges.clear();
    m_ranges.append(range);
    emit selectionChanged();
}

void Selection::setRange(int block1, int offset1, int block2, int offset2)
{
    SelectionRange range;
    range.anchorBlock = block1;
    range.anchorOffset = offset1;
    range.focusBlock = block2;
    range.focusOffset = offset2;
    range.normalize();
    setRange(range);
}

SelectionRange Selection::range() const
{
    if (m_ranges.isEmpty())
        return SelectionRange();
    return m_ranges.first();
}

QList<SelectionRange> Selection::ranges() const
{
    return m_ranges;
}

void Selection::addRange(const SelectionRange &range)
{
    // For now, just replace (simple implementation)
    setRange(range);
}

void Selection::clear()
{
    m_ranges.clear();
    emit selectionChanged();
}

void Selection::extend(int block, int offset)
{
    if (m_ranges.isEmpty()) {
        // Start a new selection from current cursor position
        // We need the cursor position; for now, assume block 0 offset 0
        setRange(0, 0, block, offset);
    } else {
        // Extend the existing selection: 只更新 focus 位置，保留 anchor 不变
        SelectionRange &first = m_ranges.first();
        first.focusBlock = block;
        first.focusOffset = offset;
        first.normalize();
        emit selectionChanged();
    }
}

bool Selection::isEmpty() const
{
    return m_ranges.isEmpty() || (m_ranges.size() == 1 && m_ranges.first().isEmpty());
}

QString Selection::selectedText() const
{
    QString result;
    if (!m_document)
        return result;
    for (const SelectionRange &range : m_ranges) {
        // Simplified: only handle single block range
        if (range.startBlock == range.endBlock) {
            Block *block = m_document->block(range.startBlock);
            ParagraphBlock *para = qobject_cast<ParagraphBlock*>(block);
            if (para) {
                QString blockText = para->text();
                int start = qMin(range.startOffset, blockText.length());
                int end = qMin(range.endOffset, blockText.length());
                if (start < end)
                    result += blockText.mid(start, end - start);
            }
        }
    }
    return result;
}

CursorPosition Selection::focusPosition() const
{
    if (m_ranges.isEmpty())
        return {-1, 0};
    return m_ranges.first().focusPosition();
}

CursorPosition Selection::anchorPosition() const
{
    if (m_ranges.isEmpty())
        return {-1, 0};
    return m_ranges.first().anchorPosition();
}

} // namespace QtWordEditor