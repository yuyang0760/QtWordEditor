/**
 * @file GenericMathSpan.cpp
 * @brief 通用公式容器数据类（简化版）实现
 */

#include "core/document/math/GenericMathSpan.h"
#include "core/document/TextSpan.h"
#include <QDebug>

namespace QtWordEditor {

GenericMathSpan::GenericMathSpan(QObject *parent)
    : MathSpan(parent)
    , m_spans()
{
}

GenericMathSpan::~GenericMathSpan()
{
    // InlineSpan 由 QObject 父子关系管理
}

InlineSpan *GenericMathSpan::clone() const
{
    GenericMathSpan *newSpan = new GenericMathSpan();
    // 注意：这里只做浅拷贝，因为 InlineSpan 的克隆需要更复杂的处理
    newSpan->m_spans = m_spans;
    return newSpan;
}

QList<InlineSpan*> GenericMathSpan::spans() const
{
    return m_spans;
}

int GenericMathSpan::spanCount() const
{
    return m_spans.size();
}

InlineSpan *GenericMathSpan::spanAt(int index) const
{
    if (index >= 0 && index < m_spans.size()) {
        return m_spans.at(index);
    }
    return nullptr;
}

void GenericMathSpan::insertSpan(int index, InlineSpan *span)
{
    if (!span || index < 0 || index > m_spans.size()) {
        return;
    }
    span->setParent(this);
    m_spans.insert(index, span);
    emit spansChanged();
    emit contentChanged();
}

void GenericMathSpan::appendSpan(InlineSpan *span)
{
    insertSpan(m_spans.size(), span);
}

void GenericMathSpan::removeSpanAt(int index)
{
    if (index < 0 || index >= m_spans.size()) {
        return;
    }
    InlineSpan *span = m_spans.takeAt(index);
    span->setParent(nullptr);
    emit spansChanged();
    emit contentChanged();
}

void GenericMathSpan::clearSpans()
{
    for (InlineSpan *span : m_spans) {
        span->setParent(nullptr);
    }
    m_spans.clear();
    emit spansChanged();
    emit contentChanged();
}

// ========== 文本编辑方法（类似 ParagraphBlock） ==========

QString GenericMathSpan::text() const
{
    QString result;
    for (const InlineSpan *span : m_spans) {
        if (span->type() == InlineSpan::Text) {
            const TextSpan *textSpan = static_cast<const TextSpan*>(span);
            result += textSpan->text();
        }
    }
    return result;
}

int GenericMathSpan::length() const
{
    int total = 0;
    for (const InlineSpan *span : m_spans) {
        total += span->length();
    }
    return total;
}

int GenericMathSpan::findInlineSpanIndex(int globalPosition, int *positionInSpan) const
{
    int totalLength = this->length();
    
    // 特殊处理：如果位置等于总长度（末尾）
    if (globalPosition == totalLength && !m_spans.isEmpty()) {
        if (positionInSpan) {
            *positionInSpan = m_spans.last()->length();
        }
        return m_spans.size() - 1;
    }
    
    int currentPos = 0;
    for (int i = 0; i < m_spans.size(); ++i) {
        int spanLength = m_spans.at(i)->length();
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
    return m_spans.size() - 1;
}

CharacterStyle GenericMathSpan::styleAt(int position) const
{
    if (m_spans.isEmpty()) {
        return CharacterStyle();
    }
    int spanIndex = findInlineSpanIndex(position);
    if (spanIndex >= 0 && spanIndex < m_spans.size()) {
        const InlineSpan *span = m_spans.at(spanIndex);
        if (span->type() == InlineSpan::Text) {
            const TextSpan *textSpan = static_cast<const TextSpan*>(span);
            return textSpan->style();
        }
    }
    return CharacterStyle();
}

void GenericMathSpan::insert(int position, const QString &text, const CharacterStyle &style)
{
    qDebug() << "[GenericMathSpan::insert] 开始, position=" << position << ", text=" << text;
    
    if (text.isEmpty())
        return;
    
    if (m_spans.isEmpty()) {
        m_spans.append(new TextSpan(text, style, this));
    } else {
        int posInSpan = 0;
        int spanIndex = findInlineSpanIndex(position, &posInSpan);
        
        if (spanIndex >= 0 && spanIndex < m_spans.size()) {
            InlineSpan *span = m_spans[spanIndex];
            
            if (span->type() == InlineSpan::Text) {
                TextSpan *textSpan = static_cast<TextSpan*>(span);
                QString spanText = textSpan->text();
                
                if (posInSpan == 0) {
                    // 在 span 开头插入
                    if (textSpan->style() == style) {
                        // 样式相同，直接前置
                        textSpan->setText(text + spanText);
                    } else {
                        // 样式不同，插入新 span
                        m_spans.insert(spanIndex, new TextSpan(text, style, this));
                    }
                } else if (posInSpan == spanText.length()) {
                    // 在 span 末尾插入
                    if (textSpan->style() == style) {
                        // 样式相同，直接追加
                        textSpan->append(text);
                    } else {
                        // 样式不同，添加新 span
                        m_spans.insert(spanIndex + 1, new TextSpan(text, style, this));
                    }
                } else {
                    // 在 span 中间插入，需要分割
                    QString before = spanText.left(posInSpan);
                    QString after = spanText.mid(posInSpan);
                    
                    m_spans.removeAt(spanIndex);
                    
                    if (!before.isEmpty()) {
                        m_spans.insert(spanIndex, new TextSpan(before, textSpan->style(), this));
                        spanIndex++;
                    }
                    
                    m_spans.insert(spanIndex, new TextSpan(text, style, this));
                    spanIndex++;
                    
                    if (!after.isEmpty()) {
                        m_spans.insert(spanIndex, new TextSpan(after, textSpan->style(), this));
                    }
                }
            } else {
                // 不是 TextSpan，直接在这个位置插入新 TextSpan
                m_spans.insert(spanIndex, new TextSpan(text, style, this));
            }
        }
    }
    
    mergeAdjacentSpans();
    qDebug() << "[GenericMathSpan::insert] 准备发出 spansChanged 和 contentChanged 信号";
    emit spansChanged();
    emit contentChanged();
    qDebug() << "[GenericMathSpan::insert] 信号已发出";
}

void GenericMathSpan::remove(int position, int length)
{
    if (length <= 0 || position < 0)
        return;
    
    int totalLen = this->length();
    if (position >= totalLen)
        return;
    
    int end = position + length;
    if (end > totalLen)
        end = totalLen;
    
    // 找到起始和结束的 span
    int posInStartSpan = 0;
    int startSpanIndex = findInlineSpanIndex(position, &posInStartSpan);
    
    int posInEndSpan = 0;
    int endSpanIndex = findInlineSpanIndex(end, &posInEndSpan);
    
    // 如果在同一个 span 内
    if (startSpanIndex == endSpanIndex) {
        InlineSpan *span = m_spans[startSpanIndex];
        if (span->type() == InlineSpan::Text) {
            TextSpan *textSpan = static_cast<TextSpan*>(span);
            QString text = textSpan->text();
            
            QString before = text.left(posInStartSpan);
            QString after = text.mid(posInEndSpan);
            
            if (before.isEmpty() && after.isEmpty()) {
                // 删除整个 span
                m_spans.removeAt(startSpanIndex);
            } else {
                textSpan->setText(before + after);
            }
        }
    } else {
        // 跨多个 span
        
        // 处理起始 span
        if (startSpanIndex >= 0 && startSpanIndex < m_spans.size()) {
            InlineSpan *startSpan = m_spans[startSpanIndex];
            if (startSpan->type() == InlineSpan::Text) {
                TextSpan *textSpan = static_cast<TextSpan*>(startSpan);
                QString before = textSpan->text().left(posInStartSpan);
                
                if (before.isEmpty()) {
                    m_spans.removeAt(startSpanIndex);
                    endSpanIndex--;
                } else {
                    textSpan->setText(before);
                    startSpanIndex++;
                }
            } else {
                // 不是 TextSpan，直接删除
                m_spans.removeAt(startSpanIndex);
                endSpanIndex--;
            }
        }
        
        // 处理结束 span
        if (endSpanIndex >= 0 && endSpanIndex < m_spans.size()) {
            InlineSpan *endSpan = m_spans[endSpanIndex];
            if (endSpan->type() == InlineSpan::Text) {
                TextSpan *textSpan = static_cast<TextSpan*>(endSpan);
                QString after = textSpan->text().mid(posInEndSpan);
                
                if (after.isEmpty()) {
                    m_spans.removeAt(endSpanIndex);
                } else {
                    textSpan->setText(after);
                }
            } else {
                // 不是 TextSpan，直接删除
                m_spans.removeAt(endSpanIndex);
            }
        }
        
        // 删除中间的 span
        while (startSpanIndex < endSpanIndex && startSpanIndex < m_spans.size()) {
            m_spans.removeAt(startSpanIndex);
            endSpanIndex--;
        }
    }
    
    mergeAdjacentSpans();
    emit spansChanged();
    emit contentChanged();
}

void GenericMathSpan::mergeAdjacentSpans()
{
    if (m_spans.size() < 2)
        return;
    
    for (int i = 0; i < m_spans.size() - 1; ++i) {
        InlineSpan *span1 = m_spans[i];
        InlineSpan *span2 = m_spans[i + 1];
        
        if (span1->type() == InlineSpan::Text && span2->type() == InlineSpan::Text) {
            TextSpan *textSpan1 = static_cast<TextSpan*>(span1);
            TextSpan *textSpan2 = static_cast<TextSpan*>(span2);
            
            if (textSpan1->style() == textSpan2->style()) {
                // 样式相同，合并
                textSpan1->setText(textSpan1->text() + textSpan2->text());
                m_spans.removeAt(i + 1);
                // 重新检查当前位置
                i--;
            }
        }
    }
}

} // namespace QtWordEditor
