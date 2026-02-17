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

    qDebug() << "ParagraphBlock::insert called, position:" << position << ", text:" << text;
    
    // 暂时简化实现：只使用单个 span
    QString currentText = this->text();
    QString newText = currentText.left(position) + text + currentText.mid(position);
    qDebug() << "  Current text:" << currentText << ", new text:" << newText;
    
    m_spans.clear();
    if (!newText.isEmpty()) {
        m_spans.append(Span(newText, style));
    }
    
    emit textChanged();
}

void ParagraphBlock::remove(int position, int length)
{
    if (length <= 0)
        return;

    qDebug() << "ParagraphBlock::remove called, position:" << position << ", length:" << length;
    
    // 暂时简化实现：只使用单个 span
    QString currentText = this->text();
    QString newText = currentText.left(position) + currentText.mid(position + length);
    qDebug() << "  Current text:" << currentText << ", new text:" << newText;
    
    m_spans.clear();
    if (!newText.isEmpty()) {
        m_spans.append(Span(newText, CharacterStyle()));
    }
    
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