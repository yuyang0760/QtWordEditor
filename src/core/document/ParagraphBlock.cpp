#include "core/document/ParagraphBlock.h"
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
    for (InlineSpan *span : other.m_inlineSpans) {
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
            const TextSpan *textSpan = qobject_cast<const TextSpan*>(span);
            if (textSpan) {
                result += textSpan->text();
            }
        }
    }
    return result;
}

void ParagraphBlock::setText(const QString &text)
{
    clearInlineSpans();
    if (!text.isEmpty()) {
        TextSpan *textSpan = new TextSpan(text, CharacterStyle(), this);
        m_inlineSpans.append(textSpan);
    }
    emit textChanged();
}

int ParagraphBlock::findInlineSpanIndex(int globalPosition, int *positionInSpan) const
{
    int totalLength = 0;
    for (const InlineSpan *span : m_inlineSpans)
        totalLength += span->length();
    
    if (globalPosition == totalLength && !m_inlineSpans.isEmpty()) {
        if (positionInSpan) {
            InlineSpan *lastSpan = m_inlineSpans.last();
            if (lastSpan->type() == InlineSpan::Text) {
                const TextSpan *textSpan = qobject_cast<const TextSpan*>(lastSpan);
                if (textSpan) {
                    *positionInSpan = textSpan->length();
                }
            } else {
                *positionInSpan = 0;
            }
        }
        return m_inlineSpans.size() - 1;
    }
    
    int currentPos = 0;
    for (int i = 0; i < m_inlineSpans.size(); ++i) {
        InlineSpan *span = m_inlineSpans.at(i);
        int spanLength = span->length();
        if (globalPosition < currentPos + spanLength) {
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
        InlineSpan *span = m_inlineSpans.at(spanIndex);
        if (span->type() == InlineSpan::Text) {
            const TextSpan *textSpan = qobject_cast<const TextSpan*>(span);
            if (textSpan) {
                return textSpan->style();
            }
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
                const TextSpan *textSpan = qobject_cast<const TextSpan*>(span);
                if (textSpan) {
                    int posInSpan = position - currentPos;
                    return textSpan->text().at(posInSpan);
                }
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
    
    LOG_DEBUG(QString("ParagraphBlock::setStyle - 开始处理: 位置%1 长度%2").arg(start).arg(length));
    
    int posInStartSpan = 0;
    int startSpanIndex = findInlineSpanIndex(start, &posInStartSpan);
    
    int posInEndSpan = 0;
    int endSpanIndex = findInlineSpanIndex(end, &posInEndSpan);
    
    if (startSpanIndex == endSpanIndex) {
        InlineSpan *span = m_inlineSpans[startSpanIndex];
        
        if (span->type() == InlineSpan::Text) {
            TextSpan *textSpan = qobject_cast<TextSpan*>(span);
            if (!textSpan) return;
            
            QString text = textSpan->text();
            
            LOG_DEBUG(QString("  同一个 span 内处理: startSpanIndex=%1, text=[%2], 长度=%3")
                .arg(startSpanIndex).arg(text).arg(text.length()));
            LOG_DEBUG(QString("    posInStartSpan=%1, posInEndSpan=%2").arg(posInStartSpan).arg(posInEndSpan));
            
            QString before = text.left(posInStartSpan);
            QString middle = text.mid(posInStartSpan, end - start);
            QString after = text.mid(posInEndSpan);
            
            LOG_DEBUG(QString("    before=[%1], middle=[%2], after=[%3]").arg(before).arg(middle).arg(after));
            
            CharacterStyle originalStyle = textSpan->style();
            
            m_inlineSpans.removeAt(startSpanIndex);
            delete textSpan;
            
            LOG_DEBUG(QString("    原始样式的加粗: %1").arg(originalStyle.bold()));
            
            if (!before.isEmpty()) {
                LOG_DEBUG(QString("    插入 before span, 加粗: %1").arg(originalStyle.bold()));
                TextSpan *beforeSpan = new TextSpan(before, originalStyle, this);
                m_inlineSpans.insert(startSpanIndex, beforeSpan);
                startSpanIndex++;
            }
            
            LOG_DEBUG("    合并样式：");
            LOG_DEBUG(QString("      originalStyle - 字体族:%1，字号:%2，加粗:%3")
                .arg(originalStyle.fontFamily())
                .arg(originalStyle.fontSize())
                .arg(originalStyle.bold()));
            LOG_DEBUG(QString("      传入的 style - 字体族:%1，字号:%2，加粗:%3")
                .arg(style.fontFamily())
                .arg(style.fontSize())
                .arg(style.bold()));
            LOG_DEBUG(QString("      传入的 style 属性标记 - 字体族:%1，字号:%2，加粗:%3")
                .arg(style.isPropertySet(CharacterStyleProperty::FontFamily))
                .arg(style.isPropertySet(CharacterStyleProperty::FontSize))
                .arg(style.isPropertySet(CharacterStyleProperty::Bold)));
            
            CharacterStyle mergedMiddleStyle = originalStyle.mergeWith(style);
            
            LOG_DEBUG(QString("      mergedMiddleStyle - 字体族:%1，字号:%2，加粗:%3")
                .arg(mergedMiddleStyle.fontFamily())
                .arg(mergedMiddleStyle.fontSize())
                .arg(mergedMiddleStyle.bold()));
            
            TextSpan *middleSpan = new TextSpan(middle, mergedMiddleStyle, this);
            m_inlineSpans.insert(startSpanIndex, middleSpan);
            startSpanIndex++;
            
            if (!after.isEmpty()) {
                LOG_DEBUG(QString("    插入 after span, 加粗: %1").arg(originalStyle.bold()));
                TextSpan *afterSpan = new TextSpan(after, originalStyle, this);
                m_inlineSpans.insert(startSpanIndex, afterSpan);
            }
            
            LOG_DEBUG(QString("  同一个 span 内处理完成，当前 spans 数量: %1").arg(m_inlineSpans.size()));
        }
    } else {
        LOG_DEBUG(QString("  跨 span 处理: startSpanIndex=%1, endSpanIndex=%2")
            .arg(startSpanIndex).arg(endSpanIndex));
        
        InlineSpan *startSpan = m_inlineSpans[startSpanIndex];
        if (startSpan->type() == InlineSpan::Text) {
            TextSpan *textStartSpan = qobject_cast<TextSpan*>(startSpan);
            if (!textStartSpan) return;
            
            QString startText = textStartSpan->text();
            QString beforeStart = startText.left(posInStartSpan);
            QString afterStart = startText.mid(posInStartSpan);
            
            LOG_DEBUG(QString("    起始 span 文本: [%1], 长度: %2").arg(startText).arg(startText.length()));
            LOG_DEBUG(QString("      beforeStart: [%1], afterStart: [%2]").arg(beforeStart).arg(afterStart));
            
            CharacterStyle originalStartStyle = textStartSpan->style();
            
            m_inlineSpans.removeAt(startSpanIndex);
            delete textStartSpan;
            
            if (!beforeStart.isEmpty()) {
                TextSpan *beforeStartSpan = new TextSpan(beforeStart, originalStartStyle, this);
                m_inlineSpans.insert(startSpanIndex, beforeStartSpan);
                startSpanIndex++;
            }
            
            CharacterStyle mergedStartStyle = originalStartStyle.mergeWith(style);
            TextSpan *afterStartSpan = new TextSpan(afterStart, mergedStartStyle, this);
            m_inlineSpans.insert(startSpanIndex, afterStartSpan);
            
            LOG_DEBUG(QString("    起始 span 处理后, startSpanIndex 现在是: %1").arg(startSpanIndex));
            LOG_DEBUG(QString("    当前 spans 数量: %1").arg(m_inlineSpans.size()));
        }
        
        int adjustedEndSpanIndex = 0;
        int posInEndSpanAdjusted = 0;
        if (end > 0 && end <= this->length()) {
            adjustedEndSpanIndex = findInlineSpanIndex(end, &posInEndSpanAdjusted);
        } else {
            adjustedEndSpanIndex = m_inlineSpans.size() - 1;
            InlineSpan *lastSpan = m_inlineSpans.last();
            if (lastSpan->type() == InlineSpan::Text) {
                const TextSpan *textSpan = qobject_cast<const TextSpan*>(lastSpan);
                if (textSpan) {
                    posInEndSpanAdjusted = textSpan->length();
                }
            }
        }
        
        LOG_DEBUG(QString("    重新计算后的 endSpanIndex: %1, posInEndSpanAdjusted: %2")
            .arg(adjustedEndSpanIndex).arg(posInEndSpanAdjusted));
        
        for (int i = startSpanIndex + 1; i < adjustedEndSpanIndex; ++i) {
            if (i >= 0 && i < m_inlineSpans.size()) {
                LOG_DEBUG(QString("      处理中间 span %1").arg(i));
                InlineSpan *span = m_inlineSpans[i];
                if (span->type() == InlineSpan::Text) {
                    TextSpan *textSpan = qobject_cast<TextSpan*>(span);
                    if (textSpan) {
                        CharacterStyle mergedMiddleStyle = textSpan->style().mergeWith(style);
                        textSpan->setStyle(mergedMiddleStyle);
                    }
                }
            }
        }
        
        if (adjustedEndSpanIndex >= 0 && adjustedEndSpanIndex < m_inlineSpans.size()) {
            InlineSpan *endSpan = m_inlineSpans[adjustedEndSpanIndex];
            if (endSpan->type() == InlineSpan::Text) {
                TextSpan *textEndSpan = qobject_cast<TextSpan*>(endSpan);
                if (!textEndSpan) return;
                
                QString endText = textEndSpan->text();
                QString beforeEnd = endText.left(posInEndSpanAdjusted);
                QString afterEnd = endText.mid(posInEndSpanAdjusted);
                
                LOG_DEBUG(QString("    结束 span 文本: [%1], 长度: %2").arg(endText).arg(endText.length()));
                LOG_DEBUG(QString("      beforeEnd: [%1], afterEnd: [%2]").arg(beforeEnd).arg(afterEnd));
                
                CharacterStyle originalEndStyle = textEndSpan->style();
                
                m_inlineSpans.removeAt(adjustedEndSpanIndex);
                delete textEndSpan;
                
                if (!beforeEnd.isEmpty()) {
                    CharacterStyle mergedEndStyle = originalEndStyle.mergeWith(style);
                    TextSpan *beforeEndSpan = new TextSpan(beforeEnd, mergedEndStyle, this);
                    m_inlineSpans.insert(adjustedEndSpanIndex, beforeEndSpan);
                    adjustedEndSpanIndex++;
                }
                
                if (!afterEnd.isEmpty()) {
                    TextSpan *afterEndSpan = new TextSpan(afterEnd, originalEndStyle, this);
                    m_inlineSpans.insert(adjustedEndSpanIndex, afterEndSpan);
                }
            }
        }
        
        LOG_DEBUG("  跨 span 处理完成");
    }
    
    LOG_DEBUG("  mergeAdjacentTextSpans 之前的 spans:");
    for (int i = 0; i < m_inlineSpans.size(); ++i) {
        InlineSpan *span = m_inlineSpans[i];
        if (span->type() == InlineSpan::Text) {
            const TextSpan *textSpan = qobject_cast<const TextSpan*>(span);
            if (textSpan) {
                LOG_DEBUG(QString("    span %1: text=[%2], 加粗:%3")
                    .arg(i).arg(textSpan->text()).arg(textSpan->style().bold()));
            }
        }
    }
    
    mergeAdjacentTextSpans();
    
    LOG_DEBUG("  mergeAdjacentTextSpans 之后的 spans:");
    for (int i = 0; i < m_inlineSpans.size(); ++i) {
        InlineSpan *span = m_inlineSpans[i];
        if (span->type() == InlineSpan::Text) {
            const TextSpan *textSpan = qobject_cast<const TextSpan*>(span);
            if (textSpan) {
                LOG_DEBUG(QString("    span %1: text=[%2], 加粗:%3")
                    .arg(i).arg(textSpan->text()).arg(textSpan->style().bold()));
            }
        }
    }
    
    LOG_DEBUG("ParagraphBlock::setStyle - 处理完成");
    emit textChanged();
}

void ParagraphBlock::insert(int position, const QString &text, const CharacterStyle &style)
{
    if (text.isEmpty())
        return;

    if (m_inlineSpans.isEmpty()) {
        TextSpan *textSpan = new TextSpan(text, style, this);
        m_inlineSpans.append(textSpan);
    } else {
        int posInSpan = 0;
        int spanIndex = findInlineSpanIndex(position, &posInSpan);
        
        if (spanIndex >= 0 && spanIndex < m_inlineSpans.size()) {
            InlineSpan *span = m_inlineSpans[spanIndex];
            
            if (span->type() == InlineSpan::Text) {
                TextSpan *textSpan = qobject_cast<TextSpan*>(span);
                if (!textSpan) return;
                
                QString spanText = textSpan->text();
                
                if (posInSpan == 0) {
                    if (textSpan->style() == style) {
                        textSpan->setText(text + spanText);
                    } else {
                        TextSpan *newSpan = new TextSpan(text, style, this);
                        m_inlineSpans.insert(spanIndex, newSpan);
                    }
                } else if (posInSpan == spanText.length()) {
                    if (textSpan->style() == style) {
                        textSpan->append(text);
                    } else {
                        TextSpan *newSpan = new TextSpan(text, style, this);
                        m_inlineSpans.insert(spanIndex + 1, newSpan);
                    }
                } else {
                    QString before = spanText.left(posInSpan);
                    QString after = spanText.mid(posInSpan);
                    
                    m_inlineSpans.removeAt(spanIndex);
                    delete textSpan;
                    
                    if (!before.isEmpty()) {
                        TextSpan *beforeSpan = new TextSpan(before, textSpan->style(), this);
                        m_inlineSpans.insert(spanIndex, beforeSpan);
                        spanIndex++;
                    }
                    
                    TextSpan *middleSpan = new TextSpan(text, style, this);
                    m_inlineSpans.insert(spanIndex, middleSpan);
                    spanIndex++;
                    
                    if (!after.isEmpty()) {
                        TextSpan *afterSpan = new TextSpan(after, textSpan->style(), this);
                        m_inlineSpans.insert(spanIndex, afterSpan);
                    }
                    
                    mergeAdjacentTextSpans();
                }
            } else {
                TextSpan *newSpan = new TextSpan(text, style, this);
                m_inlineSpans.insert(spanIndex, newSpan);
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
    
    int posInStartSpan = 0;
    int startSpanIndex = findInlineSpanIndex(position, &posInStartSpan);
    
    int posInEndSpan = 0;
    int endSpanIndex = findInlineSpanIndex(end, &posInEndSpan);
    
    if (startSpanIndex == endSpanIndex) {
        InlineSpan *span = m_inlineSpans[startSpanIndex];
        
        if (span->type() == InlineSpan::Text) {
            TextSpan *textSpan = qobject_cast<TextSpan*>(span);
            if (!textSpan) return;
            
            QString text = textSpan->text();
            QString before = text.left(posInStartSpan);
            QString after = text.mid(posInEndSpan);
            
            if (before.isEmpty() && after.isEmpty()) {
                m_inlineSpans.removeAt(startSpanIndex);
                delete textSpan;
            } else {
                textSpan->setText(before + after);
            }
        }
    } else {
        InlineSpan *startSpan = m_inlineSpans[startSpanIndex];
        if (startSpan->type() == InlineSpan::Text) {
            TextSpan *textStartSpan = qobject_cast<TextSpan*>(startSpan);
            if (!textStartSpan) return;
            
            QString startText = textStartSpan->text();
            QString beforeStart = startText.left(posInStartSpan);
            
            if (beforeStart.isEmpty()) {
                m_inlineSpans.removeAt(startSpanIndex);
                delete textStartSpan;
                endSpanIndex--;
            } else {
                textStartSpan->setText(beforeStart);
                startSpanIndex++;
            }
        }
        
        while (startSpanIndex < endSpanIndex) {
            InlineSpan *span = m_inlineSpans[startSpanIndex];
            m_inlineSpans.removeAt(startSpanIndex);
            delete span;
            endSpanIndex--;
        }
        
        if (startSpanIndex < m_inlineSpans.size()) {
            InlineSpan *endSpan = m_inlineSpans[startSpanIndex];
            if (endSpan->type() == InlineSpan::Text) {
                TextSpan *textEndSpan = qobject_cast<TextSpan*>(endSpan);
                if (!textEndSpan) return;
                
                QString endText = textEndSpan->text();
                QString afterEnd = endText.mid(posInEndSpan);
                
                if (afterEnd.isEmpty()) {
                    m_inlineSpans.removeAt(startSpanIndex);
                    delete textEndSpan;
                } else {
                    textEndSpan->setText(afterEnd);
                }
            }
        }
        
        mergeAdjacentTextSpans();
    }
    
    emit textChanged();
}



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
        emit textChanged();
    }
}

void ParagraphBlock::insertInlineSpan(int index, InlineSpan *span)
{
    if (span && index >= 0 && index <= m_inlineSpans.size()) {
        m_inlineSpans.insert(index, span);
        emit textChanged();
    }
}

void ParagraphBlock::removeInlineSpan(int index)
{
    if (index >= 0 && index < m_inlineSpans.size()) {
        delete m_inlineSpans.at(index);
        m_inlineSpans.removeAt(index);
        emit textChanged();
    }
}

void ParagraphBlock::clearInlineSpans()
{
    qDeleteAll(m_inlineSpans);
    m_inlineSpans.clear();
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
    for (const InlineSpan *span : m_inlineSpans)
        total += span->length();
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

void ParagraphBlock::mergeAdjacentTextSpans()
{
    for (int i = m_inlineSpans.size() - 1; i > 0; --i) {
        InlineSpan *prevSpan = m_inlineSpans[i - 1];
        InlineSpan *currSpan = m_inlineSpans[i];
        
        if (prevSpan->type() == InlineSpan::Text && currSpan->type() == InlineSpan::Text) {
            TextSpan *prevTextSpan = qobject_cast<TextSpan*>(prevSpan);
            TextSpan *currTextSpan = qobject_cast<TextSpan*>(currSpan);
            
            if (prevTextSpan && currTextSpan && prevTextSpan->style() == currTextSpan->style()) {
                prevTextSpan->append(currTextSpan->text());
                m_inlineSpans.removeAt(i);
                delete currTextSpan;
            }
        }
    }
}

} // namespace QtWordEditor
