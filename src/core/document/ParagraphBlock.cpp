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

int ParagraphBlock::findSpanIndex(int globalPosition, int *positionInSpan) const
{
    int totalLength = this->length();
    
    // 特殊处理：如果位置等于总长度（文档末尾）
    if (globalPosition == totalLength && !m_spans.isEmpty()) {
        if (positionInSpan) {
            *positionInSpan = m_spans.last().length();
        }
        return m_spans.size() - 1;
    }
    
    int currentPos = 0;
    for (int i = 0; i < m_spans.size(); ++i) {
        int spanLength = m_spans.at(i).length();
        if (globalPosition < currentPos + spanLength) { // 使用 < 判断位置是否在当前 span 内
            if (positionInSpan) {
                *positionInSpan = globalPosition - currentPos;
            }
            return i;
        }
        currentPos += spanLength;
    }
    if (positionInSpan) {
        *positionInSpan = 0;
    }
    return m_spans.size() - 1;
}

CharacterStyle ParagraphBlock::styleAt(int position) const
{
    if (m_spans.isEmpty()) {
        return CharacterStyle();
    }
    int spanIndex = findSpanIndex(position);
    if (spanIndex >= 0 && spanIndex < m_spans.size()) {
        return m_spans.at(spanIndex).style();
    }
    return CharacterStyle();
}

QChar ParagraphBlock::characterAt(int position) const
{
    if (position < 0 || position >= this->length()) {
        return QChar();
    }
    
    int currentPos = 0;
    for (const Span &span : m_spans) {
        int spanLength = span.length();
        if (position < currentPos + spanLength) {
            int posInSpan = position - currentPos;
            return span.text().at(posInSpan);
        }
        currentPos += spanLength;
    }
    
    return QChar();
}

void ParagraphBlock::setStyle(int start, int length, const CharacterStyle &style)
{
    if (length <= 0 || m_spans.isEmpty()) {
        return;
    }
    
    int end = start + length;
    int totalLength = this->length();
    start = qBound(0, start, totalLength);
    end = qBound(0, end, totalLength);
    
    if (start >= end) {
        return;
    }
    
    // 找到起始和结束的 span
    int posInStartSpan = 0;
    int startSpanIndex = findSpanIndex(start, &posInStartSpan);
    
    int posInEndSpan = 0;
    int endSpanIndex = findSpanIndex(end, &posInEndSpan);
    
    // 如果在同一个 span 内
    if (startSpanIndex == endSpanIndex) {
        Span &span = m_spans[startSpanIndex];
        QString text = span.text();
        
        // 分割成三个部分
        QString before = text.left(posInStartSpan);
        QString middle = text.mid(posInStartSpan, end - start);
        QString after = text.mid(posInEndSpan);
        
        // 替换原来的 span
        m_spans.removeAt(startSpanIndex);
        
        if (!before.isEmpty()) {
            m_spans.insert(startSpanIndex, Span(before, span.style()));
            startSpanIndex++;
        }
        
        m_spans.insert(startSpanIndex, Span(middle, style));
        startSpanIndex++;
        
        if (!after.isEmpty()) {
            m_spans.insert(startSpanIndex, Span(after, span.style()));
        }
    } else {
        // 处理起始 span
        Span &startSpan = m_spans[startSpanIndex];
        QString startText = startSpan.text();
        QString beforeStart = startText.left(posInStartSpan);
        QString afterStart = startText.mid(posInStartSpan);
        
        m_spans.removeAt(startSpanIndex);
        
        if (!beforeStart.isEmpty()) {
            m_spans.insert(startSpanIndex, Span(beforeStart, startSpan.style()));
            startSpanIndex++;
        }
        
        m_spans.insert(startSpanIndex, Span(afterStart, style));
        
        // 处理中间的 span（完全包含在范围内的）
        for (int i = startSpanIndex + 1; i < endSpanIndex; ++i) {
            m_spans[i].setStyle(style);
        }
        
        // 处理结束 span
        int currentEndSpanIndex = endSpanIndex;
        if (startSpanIndex < endSpanIndex) {
            currentEndSpanIndex = endSpanIndex;
        }
        
        Span &endSpan = m_spans[currentEndSpanIndex];
        QString endText = endSpan.text();
        QString beforeEnd = endText.left(posInEndSpan);
        QString afterEnd = endText.mid(posInEndSpan);
        
        m_spans.removeAt(currentEndSpanIndex);
        
        if (!beforeEnd.isEmpty()) {
            m_spans.insert(currentEndSpanIndex, Span(beforeEnd, style));
            currentEndSpanIndex++;
        }
        
        if (!afterEnd.isEmpty()) {
            m_spans.insert(currentEndSpanIndex, Span(afterEnd, endSpan.style()));
        }
    }
    
    mergeAdjacentSpans();
    emit textChanged();
}

void ParagraphBlock::insert(int position, const QString &text, const CharacterStyle &style)
{
    if (text.isEmpty())
        return;

    // qDebug() << "ParagraphBlock::insert called, position:" << position << ", text:" << text;
    
    if (m_spans.isEmpty()) {
        m_spans.append(Span(text, style));
    } else {
        int posInSpan = 0;
        int spanIndex = findSpanIndex(position, &posInSpan);
        
        if (spanIndex >= 0 && spanIndex < m_spans.size()) {
            Span &span = m_spans[spanIndex];
            QString spanText = span.text();
            
            if (posInSpan == 0) {
                // 在 span 开头插入
                if (span.style() == style) {
                    // 样式相同，直接追加
                    span.setText(text + spanText);
                } else {
                    // 样式不同，插入新 span
                    m_spans.insert(spanIndex, Span(text, style));
                }
            } else if (posInSpan == spanText.length()) {
                // 在 span 末尾插入
                if (span.style() == style) {
                    // 样式相同，直接追加
                    span.append(text);
                } else {
                    // 样式不同，添加新 span
                    m_spans.insert(spanIndex + 1, Span(text, style));
                }
            } else {
                // 在 span 中间插入，需要分割
                QString before = spanText.left(posInSpan);
                QString after = spanText.mid(posInSpan);
                
                m_spans.removeAt(spanIndex);
                
                if (!before.isEmpty()) {
                    m_spans.insert(spanIndex, Span(before, span.style()));
                    spanIndex++;
                }
                
                m_spans.insert(spanIndex, Span(text, style));
                spanIndex++;
                
                if (!after.isEmpty()) {
                    m_spans.insert(spanIndex, Span(after, span.style()));
                }
                
                mergeAdjacentSpans();
            }
        }
    }
    
    emit textChanged();
}

void ParagraphBlock::remove(int position, int length)
{
    if (length <= 0)
        return;

    // qDebug() << "ParagraphBlock::remove called, position:" << position << ", length:" << length;
    
    int totalLength = this->length();
    int end = position + length;
    position = qBound(0, position, totalLength);
    end = qBound(0, end, totalLength);
    
    if (position >= end) {
        return;
    }
    
    // 找到起始和结束的 span
    int posInStartSpan = 0;
    int startSpanIndex = findSpanIndex(position, &posInStartSpan);
    
    int posInEndSpan = 0;
    int endSpanIndex = findSpanIndex(end, &posInEndSpan);
    
    if (startSpanIndex == endSpanIndex) {
        // 在同一个 span 内
        Span &span = m_spans[startSpanIndex];
        QString text = span.text();
        QString before = text.left(posInStartSpan);
        QString after = text.mid(posInEndSpan);
        
        if (before.isEmpty() && after.isEmpty()) {
            m_spans.removeAt(startSpanIndex);
        } else {
            span.setText(before + after);
        }
    } else {
        // 处理起始 span
        Span &startSpan = m_spans[startSpanIndex];
        QString startText = startSpan.text();
        QString beforeStart = startText.left(posInStartSpan);
        
        if (beforeStart.isEmpty()) {
            m_spans.removeAt(startSpanIndex);
            endSpanIndex--;
        } else {
            startSpan.setText(beforeStart);
            startSpanIndex++;
        }
        
        // 删除中间的 span
        while (startSpanIndex < endSpanIndex) {
            m_spans.removeAt(startSpanIndex);
            endSpanIndex--;
        }
        
        // 处理结束 span
        if (startSpanIndex < m_spans.size()) {
            Span &endSpan = m_spans[startSpanIndex];
            QString endText = endSpan.text();
            QString afterEnd = endText.mid(posInEndSpan);
            
            if (afterEnd.isEmpty()) {
                m_spans.removeAt(startSpanIndex);
            } else {
                endSpan.setText(afterEnd);
            }
        }
        
        mergeAdjacentSpans();
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