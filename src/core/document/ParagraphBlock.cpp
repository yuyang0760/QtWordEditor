#include "core/document/ParagraphBlock.h"
#include "core/document/TextSpan.h"
#include "core/utils/Logger.h"
#include <QDebug>

namespace QtWordEditor {

ParagraphBlock::ParagraphBlock(QObject *parent)
    : Block(parent)
{
}

ParagraphBlock::ParagraphBlock(const ParagraphBlock &other)
    : Block(other.parent())
    , m_paragraphStyle(other.m_paragraphStyle)
{
    // 深拷贝 InlineSpan
    for (const InlineSpan *span : other.m_inlineSpans) {
        m_inlineSpans.append(span->clone());
    }
}

ParagraphBlock::~ParagraphBlock()
{
    qDeleteAll(m_inlineSpans);
    m_inlineSpans.clear();
}

QString ParagraphBlock::text() const
{
    QString result;
    for (const InlineSpan *span : m_inlineSpans) {
        if (span->type() == InlineSpan::Text) {
            const TextSpan *textSpan = static_cast<const TextSpan*>(span);
            result += textSpan->text();
        }
    }
    return result;
}

void ParagraphBlock::setText(const QString &text)
{
    qDeleteAll(m_inlineSpans);
    m_inlineSpans.clear();
    
    if (!text.isEmpty()) {
        m_inlineSpans.append(new TextSpan(text, CharacterStyle()));
    }
    emit textChanged();
}

int ParagraphBlock::findInlineSpanIndex(int globalPosition, int *positionInSpan) const
{
    int totalLength = this->length();
    
    // 特殊处理：如果位置等于总长度（文档末尾）
    if (globalPosition == totalLength && !m_inlineSpans.isEmpty()) {
        if (positionInSpan) {
            *positionInSpan = m_inlineSpans.last()->length();
        }
        return m_inlineSpans.size() - 1;
    }
    
    int currentPos = 0;
    for (int i = 0; i < m_inlineSpans.size(); ++i) {
        int spanLength = m_inlineSpans.at(i)->length();
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
    return m_inlineSpans.size() - 1;
}

CharacterStyle ParagraphBlock::styleAt(int position) const
{
    if (m_inlineSpans.isEmpty()) {
        return CharacterStyle();
    }
    int spanIndex = findInlineSpanIndex(position);
    if (spanIndex >= 0 && spanIndex < m_inlineSpans.size()) {
        const InlineSpan *span = m_inlineSpans.at(spanIndex);
        if (span->type() == InlineSpan::Text) {
            const TextSpan *textSpan = static_cast<const TextSpan*>(span);
            return textSpan->style();
        }
    }
    return CharacterStyle();
}

QChar ParagraphBlock::characterAt(int position) const
{
    if (position < 0 || position >= this->length()) {
        return QChar();
    }
    
    int currentPos = 0;
    for (const InlineSpan *span : m_inlineSpans) {
        int spanLength = span->length();
        if (position < currentPos + spanLength) {
            if (span->type() == InlineSpan::Text) {
                const TextSpan *textSpan = static_cast<const TextSpan*>(span);
                int posInSpan = position - currentPos;
                return textSpan->text().at(posInSpan);
            }
            return QChar();
        }
        currentPos += spanLength;
    }
    
    return QChar();
}

void ParagraphBlock::setStyle(int start, int length, const CharacterStyle &style)
{
    if (!validatePositionAndLength(start, length)) {
        return;
    }
    
    int end = start + length;
    
    // 找到起始和结束的 span
    int posInStartSpan = 0;
    int startSpanIndex = findInlineSpanIndex(start, &posInStartSpan);
    
    int posInEndSpan = 0;
    int endSpanIndex = findInlineSpanIndex(end, &posInEndSpan);
    
    // 如果在同一个 span 内
    if (startSpanIndex == endSpanIndex) {
        InlineSpan *span = m_inlineSpans[startSpanIndex];
        if (span->type() == InlineSpan::Text) {
            TextSpan *textSpan = static_cast<TextSpan*>(span);
            QString text = textSpan->text();
            
            // 分割成三个部分
            QString before = text.left(posInStartSpan);
            QString middle = text.mid(posInStartSpan, end - start);
            QString after = text.mid(posInEndSpan);
            
            // 保存原始样式
            CharacterStyle originalStyle = textSpan->style();
            
            // 替换原来的 span
            m_inlineSpans.removeAt(startSpanIndex);
            delete span;
            
            if (!before.isEmpty()) {
                m_inlineSpans.insert(startSpanIndex, new TextSpan(before, originalStyle));
                startSpanIndex++;
            }
            
            // 合并样式
            CharacterStyle mergedMiddleStyle = originalStyle.mergeWith(style);
            m_inlineSpans.insert(startSpanIndex, new TextSpan(middle, mergedMiddleStyle));
            startSpanIndex++;
            
            if (!after.isEmpty()) {
                m_inlineSpans.insert(startSpanIndex, new TextSpan(after, originalStyle));
            }
        }
    } else {
        // 处理起始 span
        InlineSpan *startSpan = m_inlineSpans[startSpanIndex];
        if (startSpan->type() == InlineSpan::Text) {
            TextSpan *textSpan = static_cast<TextSpan*>(startSpan);
            QString startText = textSpan->text();
            QString beforeStart = startText.left(posInStartSpan);
            QString afterStart = startText.mid(posInStartSpan);
            
            // 保存原始样式
            CharacterStyle originalStartStyle = textSpan->style();
            
            m_inlineSpans.removeAt(startSpanIndex);
            delete startSpan;
            
            if (!beforeStart.isEmpty()) {
                m_inlineSpans.insert(startSpanIndex, new TextSpan(beforeStart, originalStartStyle));
                startSpanIndex++;
            }
            
            // 合并样式
            CharacterStyle mergedStartStyle = originalStartStyle.mergeWith(style);
            m_inlineSpans.insert(startSpanIndex, new TextSpan(afterStart, mergedStartStyle));
        }
        
        // 重要！因为我们删除和插入了 span，需要重新计算 endSpanIndex
        // 重新计算当前的 endSpanIndex
        int adjustedEndSpanIndex = 0;
        int posInEndSpanAdjusted = 0;
        if (end > 0 && end <= this->length()) {
            adjustedEndSpanIndex = findInlineSpanIndex(end, &posInEndSpanAdjusted);
        } else {
            adjustedEndSpanIndex = m_inlineSpans.size() - 1;
            posInEndSpanAdjusted = m_inlineSpans.last()->length();
        }
        
        // 处理中间的 span（完全包含在范围内的）
        for (int i = startSpanIndex + 1; i < adjustedEndSpanIndex; ++i) {
            if (i >= 0 && i < m_inlineSpans.size()) {
                InlineSpan *span = m_inlineSpans[i];
                if (span->type() == InlineSpan::Text) {
                    TextSpan *textSpan = static_cast<TextSpan*>(span);
                    CharacterStyle mergedMiddleStyle = textSpan->style().mergeWith(style);
                    textSpan->setStyle(mergedMiddleStyle);
                }
            }
        }
        
        // 处理结束 span
        if (adjustedEndSpanIndex >= 0 && adjustedEndSpanIndex < m_inlineSpans.size()) {
            InlineSpan *endSpan = m_inlineSpans[adjustedEndSpanIndex];
            if (endSpan->type() == InlineSpan::Text) {
                TextSpan *textSpan = static_cast<TextSpan*>(endSpan);
                QString endText = textSpan->text();
                QString beforeEnd = endText.left(posInEndSpanAdjusted);
                QString afterEnd = endText.mid(posInEndSpanAdjusted);
                
                // 保存原始样式
                CharacterStyle originalEndStyle = textSpan->style();
                
                m_inlineSpans.removeAt(adjustedEndSpanIndex);
                delete endSpan;
                
                if (!beforeEnd.isEmpty()) {
                    CharacterStyle mergedEndStyle = originalEndStyle.mergeWith(style);
                    m_inlineSpans.insert(adjustedEndSpanIndex, new TextSpan(beforeEnd, mergedEndStyle));
                    adjustedEndSpanIndex++;
                }
                
                if (!afterEnd.isEmpty()) {
                    m_inlineSpans.insert(adjustedEndSpanIndex, new TextSpan(afterEnd, originalEndStyle));
                }
            }
        }
    }
    
    mergeAdjacentSpans();
    emit textChanged();
}

void ParagraphBlock::insert(int position, const QString &text, const CharacterStyle &style)
{
    if (text.isEmpty())
        return;
    
    if (m_inlineSpans.isEmpty()) {
        m_inlineSpans.append(new TextSpan(text, style));
    } else {
        int posInSpan = 0;
        int spanIndex = findInlineSpanIndex(position, &posInSpan);
        
        if (spanIndex >= 0 && spanIndex < m_inlineSpans.size()) {
            InlineSpan *span = m_inlineSpans[spanIndex];
            
            if (span->type() == InlineSpan::Text) {
                TextSpan *textSpan = static_cast<TextSpan*>(span);
                QString spanText = textSpan->text();
                
                if (posInSpan == 0) {
                    // 在 span 开头插入
                    if (textSpan->style() == style) {
                        // 样式相同，直接追加
                        textSpan->setText(text + spanText);
                    } else {
                        // 样式不同，插入新 span
                        m_inlineSpans.insert(spanIndex, new TextSpan(text, style));
                    }
                } else if (posInSpan == spanText.length()) {
                    // 在 span 末尾插入
                    if (textSpan->style() == style) {
                        // 样式相同，直接追加
                        textSpan->append(text);
                    } else {
                        // 样式不同，添加新 span
                        m_inlineSpans.insert(spanIndex + 1, new TextSpan(text, style));
                    }
                } else {
                    // 在 span 中间插入，需要分割
                    QString before = spanText.left(posInSpan);
                    QString after = spanText.mid(posInSpan);
                    
                    m_inlineSpans.removeAt(spanIndex);
                    delete span;
                    
                    if (!before.isEmpty()) {
                        m_inlineSpans.insert(spanIndex, new TextSpan(before, textSpan->style()));
                        spanIndex++;
                    }
                    
                    m_inlineSpans.insert(spanIndex, new TextSpan(text, style));
                    spanIndex++;
                    
                    if (!after.isEmpty()) {
                        m_inlineSpans.insert(spanIndex, new TextSpan(after, textSpan->style()));
                    }
                    
                    mergeAdjacentSpans();
                }
            } else {
                // 如果目标是 MathSpan，直接在其后插入
                m_inlineSpans.insert(spanIndex + 1, new TextSpan(text, style));
            }
        }
    }
    
    emit textChanged();
}

void ParagraphBlock::remove(int position, int length)
{
    if (!validatePositionAndLength(position, length)) {
        return;
    }

    int end = position + length;
    
    // 找到起始和结束的 span
    int posInStartSpan = 0;
    int startSpanIndex = findInlineSpanIndex(position, &posInStartSpan);
    
    int posInEndSpan = 0;
    int endSpanIndex = findInlineSpanIndex(end, &posInEndSpan);
    
    if (startSpanIndex == endSpanIndex) {
        // 在同一个 span 内
        InlineSpan *span = m_inlineSpans[startSpanIndex];
        if (span->type() == InlineSpan::Text) {
            TextSpan *textSpan = static_cast<TextSpan*>(span);
            QString text = textSpan->text();
            QString before = text.left(posInStartSpan);
            QString after = text.mid(posInEndSpan);
            
            if (before.isEmpty() && after.isEmpty()) {
                m_inlineSpans.removeAt(startSpanIndex);
                delete span;
            } else {
                textSpan->setText(before + after);
            }
        }
    } else {
        // 处理起始 span
        InlineSpan *startSpan = m_inlineSpans[startSpanIndex];
        if (startSpan->type() == InlineSpan::Text) {
            TextSpan *textSpan = static_cast<TextSpan*>(startSpan);
            QString startText = textSpan->text();
            QString beforeStart = startText.left(posInStartSpan);
            
            if (beforeStart.isEmpty()) {
                m_inlineSpans.removeAt(startSpanIndex);
                delete startSpan;
                endSpanIndex--;
            } else {
                textSpan->setText(beforeStart);
                startSpanIndex++;
            }
        }
        
        // 删除中间的 span
        while (startSpanIndex < endSpanIndex) {
            InlineSpan *span = m_inlineSpans.takeAt(startSpanIndex);
            delete span;
            endSpanIndex--;
        }
        
        // 处理结束 span
        if (startSpanIndex < m_inlineSpans.size()) {
            InlineSpan *endSpan = m_inlineSpans[startSpanIndex];
            if (endSpan->type() == InlineSpan::Text) {
                TextSpan *textSpan = static_cast<TextSpan*>(endSpan);
                QString endText = textSpan->text();
                QString afterEnd = endText.mid(posInEndSpan);
                
                if (afterEnd.isEmpty()) {
                    m_inlineSpans.removeAt(startSpanIndex);
                    delete endSpan;
                } else {
                    textSpan->setText(afterEnd);
                }
            }
        }
        
        mergeAdjacentSpans();
    }
    
    emit textChanged();
}

ParagraphStyle ParagraphBlock::paragraphStyle() const
{
    return m_paragraphStyle;
}

void ParagraphBlock::setParagraphStyle(const ParagraphStyle &style)
{
    if (m_paragraphStyle != style) {
        m_paragraphStyle = style;
    }
}

int ParagraphBlock::length() const
{
    int total = 0;
    for (const InlineSpan *span : m_inlineSpans) {
        total += span->length();
    }
    return total;
}

bool ParagraphBlock::isEmpty() const
{
    return m_inlineSpans.isEmpty() || (m_inlineSpans.size() == 1 && m_inlineSpans.first()->length() == 0);
}

Block *ParagraphBlock::clone() const
{
    ParagraphBlock *copy = new ParagraphBlock(parent());
    copy->m_paragraphStyle = m_paragraphStyle;
    copy->setBlockId(blockId());
    copy->setBoundingRect(boundingRect());
    copy->setHeight(height());
    copy->setPositionInDocument(positionInDocument());
    
    // 深拷贝 InlineSpan
    for (const InlineSpan *span : m_inlineSpans) {
        copy->m_inlineSpans.append(span->clone());
    }
    
    return copy;
}

bool ParagraphBlock::validatePositionAndLength(int& position, int& length) const
{
    if (length <= 0 || m_inlineSpans.isEmpty()) {
        return false;
    }
    
    int totalLength = this->length();
    int end = position + length;
    position = qBound(0, position, totalLength);
    end = qBound(0, end, totalLength);
    length = end - position;
    
    return length > 0;
}

void ParagraphBlock::mergeAdjacentSpans()
{
    for (int i = m_inlineSpans.size() - 1; i > 0; --i) {
        InlineSpan *prev = m_inlineSpans[i - 1];
        InlineSpan *curr = m_inlineSpans[i];
        
        if (prev->type() == InlineSpan::Text && curr->type() == InlineSpan::Text) {
            TextSpan *prevText = static_cast<TextSpan*>(prev);
            TextSpan *currText = static_cast<TextSpan*>(curr);
            
            if (prevText->style() == currText->style()) {
                prevText->append(currText->text());
                m_inlineSpans.removeAt(i);
                delete curr;
            }
        }
    }
}

// ========== InlineSpan 实现 ==========

int ParagraphBlock::inlineSpanCount() const
{
    return m_inlineSpans.size();
}

InlineSpan *ParagraphBlock::inlineSpan(int index) const
{
    if (index >= 0 && index < m_inlineSpans.size()) {
        return m_inlineSpans.at(index);
    }
    return nullptr;
}

void ParagraphBlock::addInlineSpan(InlineSpan *span)
{
    if (span) {
        m_inlineSpans.append(span);
        emit inlineSpansChanged();
    }
}

void ParagraphBlock::insertInlineSpan(int index, InlineSpan *span)
{
    if (span && index >= 0 && index <= m_inlineSpans.size()) {
        m_inlineSpans.insert(index, span);
        emit inlineSpansChanged();
    }
}

void ParagraphBlock::insertInlineSpanAtPosition(int position, InlineSpan *span)
{
    if (!span) {
        return;
    }
    
    int posInSpan = 0;
    int spanIndex = findInlineSpanIndex(position, &posInSpan);
    
    if (spanIndex >= 0 && spanIndex < m_inlineSpans.size()) {
        InlineSpan *targetSpan = m_inlineSpans[spanIndex];
        
        if (targetSpan->type() == InlineSpan::Text) {
            TextSpan *textSpan = static_cast<TextSpan*>(targetSpan);
            
            if (posInSpan == 0) {
                // 在 span 开头插入
                m_inlineSpans.insert(spanIndex, span);
            } else if (posInSpan == textSpan->length()) {
                // 在 span 末尾插入
                m_inlineSpans.insert(spanIndex + 1, span);
            } else {
                // 在 span 中间插入，需要分割
                TextSpan secondPart = textSpan->split(posInSpan);
                
                m_inlineSpans.removeAt(spanIndex);
                
                TextSpan *firstPartClone = new TextSpan(*textSpan);
                m_inlineSpans.insert(spanIndex, firstPartClone);
                spanIndex++;
                
                m_inlineSpans.insert(spanIndex, span);
                spanIndex++;
                
                TextSpan *secondPartClone = new TextSpan(secondPart);
                m_inlineSpans.insert(spanIndex, secondPartClone);
            }
        } else {
            // 如果目标是 MathSpan，直接在其后插入
            m_inlineSpans.insert(spanIndex + 1, span);
        }
    } else if (m_inlineSpans.isEmpty()) {
        // 如果没有任何 span，直接添加
        m_inlineSpans.append(span);
    } else {
        // 在末尾添加
        m_inlineSpans.append(span);
    }
    
    emit inlineSpansChanged();
}

void ParagraphBlock::removeInlineSpan(InlineSpan *span)
{
    if (span) {
        int index = m_inlineSpans.indexOf(span);
        if (index >= 0) {
            m_inlineSpans.removeAt(index);
            delete span;
            emit inlineSpansChanged();
        }
    }
}

void ParagraphBlock::removeInlineSpanAt(int index)
{
    if (index >= 0 && index < m_inlineSpans.size()) {
        InlineSpan *span = m_inlineSpans.takeAt(index);
        delete span;
        emit inlineSpansChanged();
    }
}

void ParagraphBlock::clearInlineSpans()
{
    qDeleteAll(m_inlineSpans);
    m_inlineSpans.clear();
    emit inlineSpansChanged();
}

} // namespace QtWordEditor
