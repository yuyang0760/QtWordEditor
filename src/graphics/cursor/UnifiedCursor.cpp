#include "graphics/cursor/UnifiedCursor.h"
#include "graphics/view/DocumentView.h"
#include "graphics/items/TextBlockItem.h"
#include "graphics/items/TextFragment.h"
#include "graphics/formula/MathFormulaItem.h"
#include "core/document/ParagraphBlock.h"
#include "core/document/InlineSpan.h"
#include "core/document/TextSpan.h"
#include "core/document/MathSpan.h"
#include "core/document/math/NumberMathSpan.h"
#include <QDebug>

namespace QtWordEditor {

UnifiedCursor::UnifiedCursor(DocumentView* view, QObject *parent)
    : QObject(parent)
    , m_view(view)
    , m_cursorItem(nullptr)
{
    initializeCursor();
}

UnifiedCursor::~UnifiedCursor()
{
    if (m_cursorItem) {
        delete m_cursorItem;
        m_cursorItem = nullptr;
    }
}

void UnifiedCursor::initializeCursor()
{
    if (!m_cursorItem) {
        m_cursorItem = new QGraphicsLineItem();
        m_cursorItem->setPen(QPen(Qt::black, 1));
        m_cursorItem->setZValue(100);
        
        if (m_view) {
            m_view->scene()->addItem(m_cursorItem);
        }
    }
}

void UnifiedCursor::moveLeft()
{
    if (m_state.isMath) {
        if (m_state.mathChildIndex > 0) {
            m_state.mathChildIndex--;
        } else {
            if (m_state.spanIndex > 0) {
                m_state.isMath = false;
                m_state.spanIndex--;
                InlineSpan* span = m_state.paragraph->inlineSpan(m_state.spanIndex);
                if (span && span->type() == InlineSpan::Text) {
                    TextSpan* textSpan = static_cast<TextSpan*>(span);
                    m_state.charOffset = textSpan->text().length();
                }
            }
        }
    } else {
        if (m_state.charOffset > 0) {
            m_state.charOffset--;
        } else {
            if (m_state.spanIndex > 0) {
                m_state.spanIndex--;
                InlineSpan* span = m_state.paragraph->inlineSpan(m_state.spanIndex);
                
                if (span) {
                    if (span->type() == InlineSpan::Text) {
                        TextSpan* textSpan = static_cast<TextSpan*>(span);
                        m_state.charOffset = textSpan->text().length();
                    } else if (span->type() == InlineSpan::Math) {
                        m_state.isMath = true;
                        m_state.mathSpan = static_cast<MathSpan*>(span);
                        m_state.mathChildIndex = m_state.mathSpan->childCount();
                    }
                }
            }
        }
    }
    
    updateCursor();
    emit positionChanged();
}

void UnifiedCursor::moveRight()
{
    if (m_state.isMath) {
        if (m_state.mathChildIndex < m_state.mathSpan->childCount()) {
            m_state.mathChildIndex++;
        } else {
            int nextSpanIndex = m_state.spanIndex + 1;
            if (nextSpanIndex < m_state.paragraph->inlineSpanCount()) {
                m_state.isMath = false;
                m_state.spanIndex = nextSpanIndex;
                m_state.charOffset = 0;
            }
        }
    } else {
        if (!m_state.paragraph) {
            return;
        }
        
        InlineSpan* span = m_state.paragraph->inlineSpan(m_state.spanIndex);
        if (!span) {
            return;
        }
        
        if (m_state.charOffset < span->length()) {
            m_state.charOffset++;
        } else {
            int nextSpanIndex = m_state.spanIndex + 1;
            if (nextSpanIndex < m_state.paragraph->inlineSpanCount()) {
                m_state.spanIndex = nextSpanIndex;
                m_state.charOffset = 0;
                
                InlineSpan* nextSpan = m_state.paragraph->inlineSpan(nextSpanIndex);
                if (nextSpan && nextSpan->type() == InlineSpan::Math) {
                    m_state.isMath = true;
                    m_state.mathSpan = static_cast<MathSpan*>(nextSpan);
                    m_state.mathChildIndex = 0;
                }
            }
        }
    }
    
    updateCursor();
    emit positionChanged();
}

void UnifiedCursor::moveUp()
{
    qDebug() << "UnifiedCursor::moveUp - Not implemented yet";
}

void UnifiedCursor::moveDown()
{
    qDebug() << "UnifiedCursor::moveDown - Not implemented yet";
}

void UnifiedCursor::setPosition(ParagraphBlock* paragraph, int spanIndex, int charOffset)
{
    m_state.paragraph = paragraph;
    m_state.spanIndex = spanIndex;
    m_state.charOffset = charOffset;
    m_state.isMath = false;
    m_state.mathSpan = nullptr;
    m_state.mathChildIndex = 0;
    
    updateCursor();
    emit positionChanged();
}

void UnifiedCursor::setPosition(MathSpan* mathSpan, int childIndex)
{
    if (!mathSpan) {
        return;
    }
    
    m_state.mathSpan = mathSpan;
    m_state.mathChildIndex = childIndex;
    m_state.isMath = true;
    
    updateCursor();
    emit positionChanged();
}

void UnifiedCursor::insertText(const QString& text)
{
    if (text.isEmpty()) {
        return;
    }
    
    if (m_state.isMath) {
        if (!m_state.mathSpan) {
            return;
        }
        
        if (m_state.mathChildIndex >= 0 && m_state.mathChildIndex < m_state.mathSpan->childCount()) {
            MathSpan* child = m_state.mathSpan->childAt(m_state.mathChildIndex);
            if (child && child->mathType() == MathSpan::Number) {
                NumberMathSpan* numberSpan = static_cast<NumberMathSpan*>(child);
                for (QChar ch : text) {
                    numberSpan->appendChar(ch);
                }
            }
        }
    } else {
        if (!m_state.paragraph) {
            return;
        }
        
        InlineSpan* span = m_state.paragraph->inlineSpan(m_state.spanIndex);
        if (!span || span->type() != InlineSpan::Text) {
            return;
        }
        
        TextSpan* textSpan = static_cast<TextSpan*>(span);
        textSpan->insert(m_state.charOffset, text);
        m_state.charOffset += text.length();
    }
    
    updateCursor();
    emit contentChanged();
}

void UnifiedCursor::deleteChar()
{
    if (m_state.isMath) {
        qDebug() << "UnifiedCursor::deleteChar - Math mode not implemented yet";
    } else {
        if (!m_state.paragraph) {
            return;
        }
        
        InlineSpan* span = m_state.paragraph->inlineSpan(m_state.spanIndex);
        if (!span || span->type() != InlineSpan::Text) {
            return;
        }
        
        TextSpan* textSpan = static_cast<TextSpan*>(span);
        if (m_state.charOffset < textSpan->text().length()) {
            textSpan->remove(m_state.charOffset, 1);
        }
    }
    
    updateCursor();
    emit contentChanged();
}

void UnifiedCursor::backspace()
{
    if (m_state.isMath) {
        if (m_state.mathChildIndex > 0) {
            moveLeft();
            deleteChar();
        } else {
            moveLeft();
        }
    } else {
        if (m_state.charOffset > 0) {
            m_state.charOffset--;
            deleteChar();
        } else {
            moveLeft();
        }
    }
}

CursorState UnifiedCursor::state() const
{
    return m_state;
}

bool UnifiedCursor::isInMath() const
{
    return m_state.isMath;
}

void UnifiedCursor::updateCursor()
{
    if (!m_cursorItem || !m_view) {
        return;
    }
    
    QPointF pos;
    qreal height = 20;
    
    TextBlockItem* textBlockItem = getCurrentTextBlockItem();
    if (!textBlockItem) {
        m_cursorItem->setLine(0, 0, 0, height);
        return;
    }
    
    if (m_state.isMath) {
        if (m_state.mathSpan) {
            MathFormulaItem* formulaItem = textBlockItem->getMathItemForSpan(m_state.mathSpan);
            if (formulaItem) {
                QPointF formulaPos = formulaItem->pos();
                QPointF cursorRelPos = formulaItem->cursorPosition(m_state.mathChildIndex);
                pos = textBlockItem->pos() + formulaPos + cursorRelPos;
                height = formulaItem->cursorHeight();
            }
        }
    } else {
        if (m_state.paragraph && m_state.spanIndex >= 0 && m_state.spanIndex < m_state.paragraph->inlineSpanCount()) {
            InlineSpan* span = m_state.paragraph->inlineSpan(m_state.spanIndex);
            if (span && span->type() == InlineSpan::Text) {
                TextFragment* fragment = textBlockItem->getTextFragmentForSpan(span);
                if (fragment) {
                    QPointF fragPos = fragment->pos();
                    QPointF cursorRelPos = fragment->cursorPosition(m_state.charOffset);
                    pos = textBlockItem->pos() + fragPos + cursorRelPos;
                    height = fragment->cursorHeight();
                }
            }
        }
    }
    
    m_cursorItem->setLine(pos.x(), pos.y() - height, pos.x(), pos.y());
}

QGraphicsLineItem* UnifiedCursor::cursorItem() const
{
    return m_cursorItem;
}

TextBlockItem* UnifiedCursor::getCurrentTextBlockItem() const
{
    return nullptr;
}

} // namespace QtWordEditor
