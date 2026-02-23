#include "graphics/cursor/UnifiedCursor.h"
#include "graphics/view/DocumentView.h"
#include "graphics/scene/DocumentScene.h"
#include "graphics/items/TextBlockItem.h"
#include "graphics/items/TextFragment.h"
#include "graphics/items/BaseBlockItem.h"
#include "graphics/formula/MathFormulaItem.h"
#include "core/document/ParagraphBlock.h"
#include "core/document/InlineSpan.h"
#include "core/document/TextSpan.h"
#include "core/document/MathSpan.h"
#include "core/document/math/NumberMathSpan.h"
#include "core/document/Document.h"
#include "core/document/Section.h"
#include "core/document/Page.h"
#include "core/document/CharacterStyle.h"
#include "editcontrol/cursor/Cursor.h"
#include <QDebug>

namespace QtWordEditor {

UnifiedCursor::UnifiedCursor(DocumentView* view, QObject *parent)
    : QObject(parent)
    , m_view(view)
    , m_cursorItem(nullptr)
{
    qDebug() << "UnifiedCursor 构造函数 - 开始";
    
    if (!m_cursorItem) {
        m_cursorItem = new QGraphicsRectItem();
        m_cursorItem->setBrush(QBrush(Qt::red));  // 红色填充
        m_cursorItem->setPen(QPen(Qt::red, 1));   // 红色边框
        m_cursorItem->setZValue(1000);  // 更高的Z值，确保在最上层
        qDebug() << "UnifiedCursor 构造函数 - 创建了 QGraphicsRectItem";
    }
    
    qDebug() << "UnifiedCursor 构造函数 - 完成";
}

UnifiedCursor::~UnifiedCursor()
{
    qDebug() << "UnifiedCursor 析构函数 - 开始";
    
    if (m_cursorItem) {
        if (m_cursorItem->scene()) {
            m_cursorItem->scene()->removeItem(m_cursorItem);
        }
        delete m_cursorItem;
        m_cursorItem = nullptr;
    }
    
    qDebug() << "UnifiedCursor 析构函数 - 完成";
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

int UnifiedCursor::getCurrentBlockIndex() const
{
    if (!m_state.paragraph || !m_view) {
        return -1;
    }
    
    DocumentScene* scene = qobject_cast<DocumentScene*>(m_view->scene());
    if (!scene || !scene->document()) {
        return -1;
    }
    
    Document* doc = scene->document();
    
    for (int i = 0; i < doc->blockCount(); ++i) {
        if (doc->block(i) == m_state.paragraph) {
            return i;
        }
    }
    
    return -1;
}

void UnifiedCursor::moveUp()
{
    int blockIndex = getCurrentBlockIndex();
    if (blockIndex > 0) {
        DocumentScene* scene = qobject_cast<DocumentScene*>(m_view->scene());
        if (!scene || !scene->document()) {
            return;
        }
        
        Document* doc = scene->document();
        Block* prevBlock = doc->block(blockIndex - 1);
        ParagraphBlock* paragraph = qobject_cast<ParagraphBlock*>(prevBlock);
        
        if (paragraph) {
            setPosition(paragraph, 0, 0);
        }
    }
}

void UnifiedCursor::moveDown()
{
    int blockIndex = getCurrentBlockIndex();
    DocumentScene* scene = qobject_cast<DocumentScene*>(m_view->scene());
    if (!scene || !scene->document()) {
        return;
    }
    
    Document* doc = scene->document();
    if (blockIndex >= 0 && blockIndex < doc->blockCount() - 1) {
        Block* nextBlock = doc->block(blockIndex + 1);
        ParagraphBlock* paragraph = qobject_cast<ParagraphBlock*>(nextBlock);
        
        if (paragraph) {
            setPosition(paragraph, 0, 0);
        }
    }
}

void UnifiedCursor::moveToStartOfLine()
{
    if (!m_state.paragraph) {
        return;
    }
    
    m_state.spanIndex = 0;
    m_state.charOffset = 0;
    m_state.isMath = false;
    m_state.mathSpan = nullptr;
    m_state.mathChildIndex = 0;
    
    updateCursor();
    emit positionChanged();
}

void UnifiedCursor::moveToEndOfLine()
{
    if (!m_state.paragraph) {
        return;
    }
    
    int spanCount = m_state.paragraph->inlineSpanCount();
    if (spanCount > 0) {
        m_state.spanIndex = spanCount - 1;
        InlineSpan* lastSpan = m_state.paragraph->inlineSpan(m_state.spanIndex);
        if (lastSpan) {
            if (lastSpan->type() == InlineSpan::Text) {
                TextSpan* textSpan = static_cast<TextSpan*>(lastSpan);
                m_state.charOffset = textSpan->text().length();
            } else {
                m_state.charOffset = 1;
            }
        }
    } else {
        m_state.spanIndex = 0;
        m_state.charOffset = 0;
    }
    m_state.isMath = false;
    m_state.mathSpan = nullptr;
    m_state.mathChildIndex = 0;
    
    updateCursor();
    emit positionChanged();
}

void UnifiedCursor::moveToStartOfDocument()
{
    if (!m_view) {
        return;
    }
    
    DocumentScene* scene = qobject_cast<DocumentScene*>(m_view->scene());
    if (!scene || !scene->document()) {
        return;
    }
    
    Document* doc = scene->document();
    Section* section = doc->section(0);
    if (!section || section->pageCount() <= 0) {
        return;
    }
    
    Page* page = section->page(0);
    if (!page || page->blockCount() <= 0) {
        return;
    }
    
    Block* firstBlock = page->block(0);
    ParagraphBlock* paragraph = qobject_cast<ParagraphBlock*>(firstBlock);
    if (!paragraph) {
        return;
    }
    
    setPosition(paragraph, 0, 0);
    
    qDebug() << "UnifiedCursor::moveToStartOfDocument - moved to first paragraph";
}

void UnifiedCursor::moveToEndOfDocument()
{
    if (!m_view) {
        return;
    }
    
    DocumentScene* scene = qobject_cast<DocumentScene*>(m_view->scene());
    if (!scene || !scene->document()) {
        return;
    }
    
    Document* doc = scene->document();
    Section* section = doc->section(0);
    if (!section || section->pageCount() <= 0) {
        return;
    }
    
    Page* page = section->page(0);
    if (!page || page->blockCount() <= 0) {
        return;
    }
    
    Block* lastBlock = page->block(page->blockCount() - 1);
    ParagraphBlock* paragraph = qobject_cast<ParagraphBlock*>(lastBlock);
    if (!paragraph) {
        return;
    }
    
    int spanCount = paragraph->inlineSpanCount();
    int spanIndex = spanCount > 0 ? spanCount - 1 : 0;
    int charOffset = 0;
    
    if (spanCount > 0) {
        InlineSpan* span = paragraph->inlineSpan(spanIndex);
        if (span && span->type() == InlineSpan::Text) {
            TextSpan* textSpan = static_cast<TextSpan*>(span);
            charOffset = textSpan->text().length();
        } else {
            charOffset = 1;
        }
    }
    
    setPosition(paragraph, spanIndex, charOffset);
    
    qDebug() << "UnifiedCursor::moveToEndOfDocument - moved to last paragraph";
}

void UnifiedCursor::setPositionFromScenePoint(const QPointF& scenePos)
{
    if (!m_view) {
        return;
    }
    
    DocumentScene* scene = qobject_cast<DocumentScene*>(m_view->scene());
    if (!scene || !scene->document()) {
        return;
    }
    
    // 使用 DocumentScene 的方法获取光标位置（块索引和偏移）
    CursorPosition cursorPos = scene->cursorPositionAt(scenePos);
    
    // 获取文档
    Document* doc = scene->document();
    if (!doc) {
        return;
    }
    
    // 根据全局块索引获取块
    Block* block = doc->block(cursorPos.blockIndex);
    if (!block) {
        return;
    }
    
    // 转换为 ParagraphBlock
    ParagraphBlock* paragraph = qobject_cast<ParagraphBlock*>(block);
    if (!paragraph) {
        return;
    }
    
    // 将全局 offset 转换为 span 索引和 char 偏移
    int positionInSpan = 0;
    int spanIndex = paragraph->findInlineSpanIndex(cursorPos.offset, &positionInSpan);
    
    if (spanIndex >= 0) {
        InlineSpan* span = paragraph->inlineSpan(spanIndex);
        if (span) {
            if (span->type() == InlineSpan::Text) {
                // 文本 span，直接设置位置
                setPosition(paragraph, spanIndex, positionInSpan);
            } else if (span->type() == InlineSpan::Math) {
                // 数学 span，设置为数学模式
                m_state.paragraph = paragraph;
                m_state.spanIndex = spanIndex;
                m_state.isMath = true;
                m_state.mathSpan = static_cast<MathSpan*>(span);
                m_state.mathChildIndex = 0;
                m_state.charOffset = 0;
                
                updateCursor();
                emit positionChanged();
            }
        }
    } else {
        // 未找到 span，定位到段落开始
        setPosition(paragraph, 0, 0);
    }
    
    qDebug() << "UnifiedCursor::setPositionFromScenePoint - scenePos:" << scenePos
             << "blockIndex:" << cursorPos.blockIndex
             << "offset:" << cursorPos.offset
             << "spanIndex:" << spanIndex
             << "positionInSpan:" << positionInSpan;
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
    qDebug() << "[DEBUG] UnifiedCursor::insertText (no style) - text:" << text;
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
            qDebug() << "[DEBUG] UnifiedCursor::insertText - m_state.paragraph is null!";
            return;
        }
        
        InlineSpan* span = m_state.paragraph->inlineSpan(m_state.spanIndex);
        if (!span || span->type() != InlineSpan::Text) {
            qDebug() << "[DEBUG] UnifiedCursor::insertText - span is not Text type!";
            return;
        }
        
        TextSpan* textSpan = static_cast<TextSpan*>(span);
        qDebug() << "[DEBUG] UnifiedCursor::insertText - before insert, text:" << textSpan->text();
        textSpan->insert(m_state.charOffset, text);
        qDebug() << "[DEBUG] UnifiedCursor::insertText - after insert, text:" << textSpan->text();
        m_state.charOffset += text.length();
    }
    
    updateCursor();
    emit contentChanged();
}

void UnifiedCursor::insertText(const QString& text, const CharacterStyle& style)
{
    qDebug() << "[DEBUG] UnifiedCursor::insertText (with style) - text:" << text;
    if (text.isEmpty()) {
        return;
    }
    
    if (m_state.isMath) {
        // 在数学模式下，暂时不处理带样式的插入
        insertText(text);
        return;
    }
    
    if (!m_state.paragraph) {
        return;
    }
    
    // 计算全局位置
    int globalOffset = 0;
    for (int i = 0; i < m_state.spanIndex; ++i) {
        InlineSpan* span = m_state.paragraph->inlineSpan(i);
        if (span) {
            globalOffset += span->length();
        }
    }
    globalOffset += m_state.charOffset;
    
    // 使用 ParagraphBlock 的 insert 方法，这样会自动处理样式
    m_state.paragraph->insert(globalOffset, text, style);
    
    // 重新计算 span 索引和 char 偏移
    int positionInSpan = 0;
    int newSpanIndex = m_state.paragraph->findInlineSpanIndex(globalOffset + text.length(), &positionInSpan);
    
    if (newSpanIndex >= 0) {
        setPosition(m_state.paragraph, newSpanIndex, positionInSpan);
    }
    
    updateCursor();
    emit contentChanged();
}

void UnifiedCursor::deleteChar()
{
    qDebug() << "[DEBUG] UnifiedCursor::deleteChar";
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
        qDebug() << "[DEBUG] UnifiedCursor::deleteChar - before delete, text:" << textSpan->text();
        if (m_state.charOffset < textSpan->text().length()) {
            textSpan->remove(m_state.charOffset, 1);
        }
        qDebug() << "[DEBUG] UnifiedCursor::deleteChar - after delete, text:" << textSpan->text();
    }
    
    updateCursor();
    emit contentChanged();
}

void UnifiedCursor::deletePreviousChar()
{
    qDebug() << "[DEBUG] UnifiedCursor::deletePreviousChar";
    backspace();
}

void UnifiedCursor::backspace()
{
    qDebug() << "[DEBUG] UnifiedCursor::backspace";
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
    qDebug() << "UnifiedCursor::updateCursor() - 开始 (无参数)";
    
    if (!m_cursorItem || !m_view) {
        qDebug() << "UnifiedCursor::updateCursor() - m_cursorItem 或 m_view 是 null!";
        return;
    }
    
    // 获取当前 TextBlockItem
    TextBlockItem* textBlockItem = getCurrentTextBlockItem();
    if (!textBlockItem) {
        qDebug() << "UnifiedCursor::updateCursor() - 获取不到 TextBlockItem";
        return;
    }
    
    QPointF cursorPos;
    qreal cursorHeight = 20;
    
    if (m_state.isMath) {
        // 在公式内
        qDebug() << "UnifiedCursor::updateCursor() - 在公式内";
        
        if (m_state.mathSpan) {
            MathFormulaItem* mathItem = textBlockItem->getMathItemForSpan(m_state.mathSpan);
            if (mathItem) {
                QPointF localPos = mathItem->cursorPosition(m_state.mathChildIndex);
                cursorPos = textBlockItem->mapToScene(mathItem->mapToParent(localPos));
                cursorHeight = mathItem->cursorHeight();
                qDebug() << "UnifiedCursor::updateCursor() - 公式内光标位置:" << cursorPos << "高度:" << cursorHeight;
            }
        }
    } else {
        // 在文本内
        qDebug() << "UnifiedCursor::updateCursor() - 在文本内";
        
        if (m_state.spanIndex >= 0 && m_state.spanIndex < m_state.paragraph->inlineSpanCount()) {
            InlineSpan* span = m_state.paragraph->inlineSpan(m_state.spanIndex);
            if (span && span->type() == InlineSpan::Text) {
                TextFragment* textFragment = textBlockItem->getTextFragmentForSpan(span);
                if (textFragment) {
                    QPointF localPos = textFragment->cursorPosition(m_state.charOffset);
                    cursorPos = textBlockItem->mapToScene(textFragment->mapToParent(localPos));
                    cursorHeight = textFragment->cursorHeight();
                    qDebug() << "UnifiedCursor::updateCursor() - 文本内光标位置:" << cursorPos << "高度:" << cursorHeight;
                }
            }
        }
    }
    
    // 设置光标位置
    qreal cursorWidth = 2;
    m_cursorItem->setRect(cursorPos.x(), cursorPos.y(), cursorWidth, cursorHeight);
    
    qDebug() << "UnifiedCursor::updateCursor() - 最终光标矩形:" << m_cursorItem->rect();
}

void UnifiedCursor::updateCursor(const QPointF& position, qreal height)
{
    if (!m_cursorItem) {
        return;
    }
    
    // 绘制一个细长的红色矩形，更像光标
    qreal cursorWidth = 2;  // 光标宽度为 2 像素
    m_cursorItem->setRect(position.x(), position.y(), cursorWidth, height);
    
    qDebug() << "UnifiedCursor::updateCursor(position, height) - 光标位置:" << position << "高度:" << height;
}

QGraphicsRectItem* UnifiedCursor::cursorItem() const
{
    return m_cursorItem;
}

TextBlockItem* UnifiedCursor::getCurrentTextBlockItem() const
{
    if (!m_view || !m_state.paragraph) {
        return nullptr;
    }
    
    DocumentScene* scene = qobject_cast<DocumentScene*>(m_view->scene());
    if (!scene) {
        return nullptr;
    }
    
    // 通过 DocumentScene 获取对应的 BaseBlockItem
    BaseBlockItem* baseItem = scene->blockItemForBlock(m_state.paragraph);
    if (!baseItem) {
        return nullptr;
    }
    
    // 转换为 TextBlockItem（使用 dynamic_cast，因为 BaseBlockItem 不是 QObject）
    return dynamic_cast<TextBlockItem*>(baseItem);
}

} // namespace QtWordEditor
