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
    if (!validatePositionAndLength(start, length)) {
        return;
    }
    
    int end = start + length;
    
    LOG_DEBUG(QString("ParagraphBlock::setStyle - 开始处理: 位置%1 长度%2").arg(start).arg(length));
    
    // 找到起始和结束的 span
    int posInStartSpan = 0;
    int startSpanIndex = findSpanIndex(start, &posInStartSpan);
    
    int posInEndSpan = 0;
    int endSpanIndex = findSpanIndex(end, &posInEndSpan);
    
    // 如果在同一个 span 内
    if (startSpanIndex == endSpanIndex) {
        Span &span = m_spans[startSpanIndex];
        QString text = span.text();
        
        LOG_DEBUG(QString("  同一个 span 内处理: startSpanIndex=%1, text=[%2], 长度=%3")
            .arg(startSpanIndex).arg(text).arg(text.length()));
        LOG_DEBUG(QString("    posInStartSpan=%1, posInEndSpan=%2").arg(posInStartSpan).arg(posInEndSpan));
        
        // 分割成三个部分
        QString before = text.left(posInStartSpan);
        QString middle = text.mid(posInStartSpan, end - start);
        QString after = text.mid(posInEndSpan);
        
        LOG_DEBUG(QString("    before=[%1], middle=[%2], after=[%3]").arg(before).arg(middle).arg(after));
        
        // 保存原始样式的副本！防止隐式共享问题！必须在删除之前获取！
        CharacterStyle originalStyle = span.style();
        
        // 替换原来的 span
        m_spans.removeAt(startSpanIndex);
        
        LOG_DEBUG(QString("    原始样式的加粗: %1").arg(originalStyle.bold()));
        
        if (!before.isEmpty()) {
            LOG_DEBUG(QString("    插入 before span, 加粗: %1").arg(originalStyle.bold()));
            m_spans.insert(startSpanIndex, Span(before, originalStyle));
            startSpanIndex++;
        }
        
        // ========== 修改：合并样式而不是直接替换 ==========
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
        
        m_spans.insert(startSpanIndex, Span(middle, mergedMiddleStyle));
        startSpanIndex++;
        
        if (!after.isEmpty()) {
            LOG_DEBUG(QString("    插入 after span, 加粗: %1").arg(originalStyle.bold()));
            m_spans.insert(startSpanIndex, Span(after, originalStyle));
        }
        
        LOG_DEBUG(QString("  同一个 span 内处理完成，当前 spans 数量: %1").arg(m_spans.size()));
    } else {
        LOG_DEBUG(QString("  跨 span 处理: startSpanIndex=%1, endSpanIndex=%2")
            .arg(startSpanIndex).arg(endSpanIndex));
        
        // 处理起始 span
        Span &startSpan = m_spans[startSpanIndex];
        QString startText = startSpan.text();
        QString beforeStart = startText.left(posInStartSpan);
        QString afterStart = startText.mid(posInStartSpan);
        
        LOG_DEBUG(QString("    起始 span 文本: [%1], 长度: %2").arg(startText).arg(startText.length()));
        LOG_DEBUG(QString("      beforeStart: [%1], afterStart: [%2]").arg(beforeStart).arg(afterStart));
        
        // 保存原始样式的副本！防止隐式共享问题！
        CharacterStyle originalStartStyle = startSpan.style();
        
        m_spans.removeAt(startSpanIndex);
        
        if (!beforeStart.isEmpty()) {
            m_spans.insert(startSpanIndex, Span(beforeStart, originalStartStyle));
            startSpanIndex++;
        }
        
        // ========== 修改：合并样式而不是直接替换 ==========
        CharacterStyle mergedStartStyle = originalStartStyle.mergeWith(style);
        m_spans.insert(startSpanIndex, Span(afterStart, mergedStartStyle));
        
        LOG_DEBUG(QString("    起始 span 处理后, startSpanIndex 现在是: %1").arg(startSpanIndex));
        LOG_DEBUG(QString("    当前 spans 数量: %1").arg(m_spans.size()));
        
        // 重要！因为我们删除和插入了 span，需要重新计算 endSpanIndex
        // 重新计算当前的 endSpanIndex
        int adjustedEndSpanIndex = 0;
        int posInEndSpanAdjusted = 0;
        if (end > 0 && end <= this->length()) {
            adjustedEndSpanIndex = findSpanIndex(end, &posInEndSpanAdjusted);
        } else {
            adjustedEndSpanIndex = m_spans.size() - 1;
            posInEndSpanAdjusted = m_spans.last().length();
        }
        
        LOG_DEBUG(QString("    重新计算后的 endSpanIndex: %1, posInEndSpanAdjusted: %2")
            .arg(adjustedEndSpanIndex).arg(posInEndSpanAdjusted));
        
        // 处理中间的 span（完全包含在范围内的）
        // 注意：现在中间 span 的索引范围是 startSpanIndex + 1 到 adjustedEndSpanIndex - 1
        for (int i = startSpanIndex + 1; i < adjustedEndSpanIndex; ++i) {
            if (i >= 0 && i < m_spans.size()) {
                LOG_DEBUG(QString("      处理中间 span %1").arg(i));
                CharacterStyle mergedMiddleStyle = m_spans[i].style().mergeWith(style);
                m_spans[i].setStyle(mergedMiddleStyle);
            }
        }
        
        // 处理结束 span
        if (adjustedEndSpanIndex >= 0 && adjustedEndSpanIndex < m_spans.size()) {
            Span &endSpan = m_spans[adjustedEndSpanIndex];
            QString endText = endSpan.text();
            QString beforeEnd = endText.left(posInEndSpanAdjusted);
            QString afterEnd = endText.mid(posInEndSpanAdjusted);
            
            LOG_DEBUG(QString("    结束 span 文本: [%1], 长度: %2").arg(endText).arg(endText.length()));
            LOG_DEBUG(QString("      beforeEnd: [%1], afterEnd: [%2]").arg(beforeEnd).arg(afterEnd));
            
            // 保存原始样式引用
            CharacterStyle originalEndStyle = endSpan.style();
            
            m_spans.removeAt(adjustedEndSpanIndex);
            
            if (!beforeEnd.isEmpty()) {
                CharacterStyle mergedEndStyle = originalEndStyle.mergeWith(style);
                m_spans.insert(adjustedEndSpanIndex, Span(beforeEnd, mergedEndStyle));
                adjustedEndSpanIndex++;
            }
            
            if (!afterEnd.isEmpty()) {
                m_spans.insert(adjustedEndSpanIndex, Span(afterEnd, originalEndStyle));
            }
        }
        
        LOG_DEBUG("  跨 span 处理完成");
    }
    
    LOG_DEBUG("  mergeAdjacentSpans 之前的 spans:");
    for (int i = 0; i < m_spans.size(); ++i) {
        LOG_DEBUG(QString("    span %1: text=[%2], 加粗:%3")
            .arg(i).arg(m_spans[i].text()).arg(m_spans[i].style().bold()));
    }
    
    mergeAdjacentSpans();
    
    LOG_DEBUG("  mergeAdjacentSpans 之后的 spans:");
    for (int i = 0; i < m_spans.size(); ++i) {
        LOG_DEBUG(QString("    span %1: text=[%2], 加粗:%3")
            .arg(i).arg(m_spans[i].text()).arg(m_spans[i].style().bold()));
    }
    
    LOG_DEBUG("ParagraphBlock::setStyle - 处理完成");
    emit textChanged();
}

void ParagraphBlock::insert(int position, const QString &text, const CharacterStyle &style)
{
    if (text.isEmpty())
        return;

  //  QDebug() << "ParagraphBlock::insert - 插入文本，位置:" << position << "，文本:" << text;
    
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
    if (!validatePositionAndLength(position, length)) {
        return;
    }

    int end = position + length;
    
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

bool ParagraphBlock::validatePositionAndLength(int& position, int& length) const
{
    if (length <= 0 || m_spans.isEmpty()) {
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