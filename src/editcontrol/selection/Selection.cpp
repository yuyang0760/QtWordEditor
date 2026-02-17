#include "include/editcontrol/selection/Selection.h"
#include "include/core/document/Document.h"
#include "include/core/document/Block.h"
#include "include/core/document/ParagraphBlock.h"
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
    SelectionRange range{block1, offset1, block2, offset2};
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
        // Extend the existing selection
        SelectionRange &first = m_ranges.first();
        first.endBlock = block;
        first.endOffset = offset;
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

} // namespace QtWordEditor