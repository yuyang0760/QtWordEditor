#include "core/document/ParagraphBlock.h"
#include <QDebug>

namespace QtWordEditor {

ParagraphBlock::ParagraphBlock(QObject *parent)
    : Block(parent)
{
}

ParagraphBlock::ParagraphBlock(const ParagraphBlock &other)
    : Block(other.parent())
    , m_spans(other.m_spans)
    , m_paragraphStyle(other.m_paragraphStyle)
{
    // Note: clone() should be used for deep copy
}

ParagraphBlock::~ParagraphBlock()
{
}

QString ParagraphBlock::text() const
{
    QString result;
    for (const Span &span : m_spans)
        result += span.text();
    return result;
}

void ParagraphBlock::setText(const QString &text)
{
    m_spans.clear();
    if (!text.isEmpty()) {
        m_spans.append(Span(text, CharacterStyle()));
    }
    emit textChanged();
}

void ParagraphBlock::insert(int position, const QString &text, const CharacterStyle &style)
{
    if (text.isEmpty())
        return;

    // Find the span containing the position
    int currentPos = 0;
    for (int i = 0; i < m_spans.size(); ++i) {
        Span &span = m_spans[i];
        int len = span.length();
        if (position >= currentPos && position <= currentPos + len) {
            // Split the span at the insertion point
            int offset = position - currentPos;
            Span second = span.split(offset);
            // Insert new span(s) between the two parts
            if (!text.isEmpty()) {
                Span newSpan(text, style);
                m_spans.insert(i + 1, newSpan);
                // If second part is not empty, insert it after newSpan
                if (second.length() > 0) {
                    m_spans.insert(i + 2, second);
                }
            } else {
                // If text empty, just split (maybe not needed)
                if (second.length() > 0) {
                    m_spans.insert(i + 1, second);
                }
            }
            mergeAdjacentSpans();
            emit textChanged();
            return;
        }
        currentPos += len;
    }
    // If position is at end, append a new span
    if (position >= currentPos) {
        m_spans.append(Span(text, style));
        emit textChanged();
    }
}

void ParagraphBlock::remove(int position, int length)
{
    if (length <= 0)
        return;

    // Simple implementation: remove characters from spans
    // This is a simplified version; a full implementation would need to handle span splitting and merging.
    int currentPos = 0;
    for (int i = 0; i < m_spans.size(); ++i) {
        Span &span = m_spans[i];
        int len = span.length();
        if (position < currentPos + len) {
            int offset = position - currentPos;
            int removeLen = qMin(length, len - offset);
            span.remove(offset, removeLen);
            length -= removeLen;
            position += removeLen;
            if (span.length() == 0) {
                m_spans.removeAt(i);
                --i;
            }
            if (length == 0)
                break;
        }
        currentPos += len;
    }
    mergeAdjacentSpans();
    emit textChanged();
}

int ParagraphBlock::spanCount() const
{
    return m_spans.size();
}

Span ParagraphBlock::span(int index) const
{
    if (index >= 0 && index < m_spans.size())
        return m_spans.at(index);
    return Span();
}

void ParagraphBlock::addSpan(const Span &span)
{
    m_spans.append(span);
    emit textChanged();
}

void ParagraphBlock::setSpan(int index, const Span &span)
{
    if (index >= 0 && index < m_spans.size()) {
        m_spans[index] = span;
        emit textChanged();
    }
}

ParagraphStyle ParagraphBlock::paragraphStyle() const
{
    return m_paragraphStyle;
}

void ParagraphBlock::setParagraphStyle(const ParagraphStyle &style)
{
    if (m_paragraphStyle != style) {
        m_paragraphStyle = style;
        // Emit style change signal if needed
    }
}

int ParagraphBlock::length() const
{
    int total = 0;
    for (const Span &span : m_spans)
        total += span.length();
    return total;
}

bool ParagraphBlock::isEmpty() const
{
    return m_spans.isEmpty() || (m_spans.size() == 1 && m_spans.first().length() == 0);
}

Block *ParagraphBlock::clone() const
{
    ParagraphBlock *copy = new ParagraphBlock(parent());
    copy->m_spans = m_spans;
    copy->m_paragraphStyle = m_paragraphStyle;
    copy->setBlockId(blockId());
    copy->setBoundingRect(boundingRect());
    copy->setHeight(height());
    copy->setPositionInDocument(positionInDocument());
    return copy;
}

void ParagraphBlock::mergeAdjacentSpans()
{
    for (int i = m_spans.size() - 1; i > 0; --i) {
        Span &prev = m_spans[i - 1];
        Span &curr = m_spans[i];
        if (prev.style() == curr.style()) {
            prev.append(curr.text());
            m_spans.removeAt(i);
        }
    }
}

} // namespace QtWordEditor