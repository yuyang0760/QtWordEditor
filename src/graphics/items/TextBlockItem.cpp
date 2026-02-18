#include "graphics/items/TextBlockItem.h"
#include "core/document/ParagraphBlock.h"
#include "core/document/CharacterStyle.h"
#include "core/utils/Constants.h"
#include <QDebug>
#include <QFont>
#include <QFontMetrics>
#include <QTextDocument>
#include <QTextOption>
#include <QTextCursor>
#include <QTextCharFormat>

namespace QtWordEditor {

TextBlockItem::TextBlockItem(ParagraphBlock *block, QGraphicsItem *parent)
    : BaseBlockItem(block, parent)
    , m_textItem(new QGraphicsTextItem(this))
    , m_textWidth(Constants::PAGE_WIDTH - 2 * Constants::PAGE_MARGIN)
{
    setFlag(QGraphicsItem::ItemIsSelectable, false);
    setFlag(QGraphicsItem::ItemIsFocusable, false);
    
    initializeTextItem();
    
    // 从 ParagraphBlock 读取初始文本（支持富文本）
    if (block) {
        applyRichTextFromBlock();
    }
    
    updateBoundingRect();
}

TextBlockItem::~TextBlockItem()
{
}

void TextBlockItem::initializeTextItem()
{
    // 设置文本宽度以启用自动换行
    m_textItem->setTextWidth(m_textWidth);
    
    // 禁用 QGraphicsTextItem 的默认文档边距
    m_textItem->document()->setDocumentMargin(0);
    
    // 验证边距是否正确设置
    // qDebug() << "TextBlockItem::initializeTextItem - 文档边距:" << m_textItem->document()->documentMargin();
    
    // 设置文本换行选项：按字符换行，不是单词边界
    QTextOption option = m_textItem->document()->defaultTextOption();
    option.setWrapMode(QTextOption::WrapAnywhere);
    m_textItem->document()->setDefaultTextOption(option);
    
    // 设置默认字体
    QFont font;
    font.setFamily(Constants::DefaultFontFamily);
    font.setPointSize(Constants::DefaultFontSize);
    m_textItem->setFont(font);
    
    // 内部 m_textItem 相对于 TextBlockItem 的位置是 (0,0)
    m_textItem->setPos(0, 0);
    // qDebug() << "TextBlockItem::initializeTextItem - 文本项位置:" << m_textItem->pos();
}

QGraphicsTextItem *TextBlockItem::textItem() const
{
    return m_textItem;
}

void TextBlockItem::setTextWidth(qreal width)
{
    if (m_textWidth != width) {
        m_textWidth = width;
        m_textItem->setTextWidth(width);
        updateBoundingRect();
    }
}

qreal TextBlockItem::textWidth() const
{
    return m_textWidth;
}

void TextBlockItem::setFont(const QFont &font)
{
    m_textItem->setFont(font);
    updateBoundingRect();
}

QFont TextBlockItem::font() const
{
    return m_textItem->font();
}

void TextBlockItem::setPlainText(const QString &text)
{
    m_textItem->setPlainText(text);
    updateBoundingRect();
}

QString TextBlockItem::toPlainText() const
{
    return m_textItem->toPlainText();
}

QRectF TextBlockItem::boundingRect() const
{
    return QGraphicsRectItem::boundingRect();
}

void TextBlockItem::updateGeometry()
{
    updateBoundingRect();
}

void TextBlockItem::updateBoundingRect()
{
    QRectF textRect = m_textItem->boundingRect();
    setRect(0, 0, textRect.width(), textRect.height());
}

void TextBlockItem::updateBlock()
{
    ParagraphBlock *para = qobject_cast<ParagraphBlock*>(m_block);
    if (!para)
        return;
    
    applyRichTextFromBlock();
    updateBoundingRect();
}

void TextBlockItem::applyRichTextFromBlock()
{
    ParagraphBlock *para = qobject_cast<ParagraphBlock*>(m_block);
    if (!para)
        return;
    
    QTextDocument *doc = m_textItem->document();
    QTextCursor cursor(doc);
    cursor.select(QTextCursor::Document);
    cursor.removeSelectedText();
    
    for (int i = 0; i < para->spanCount(); ++i) {
        Span span = para->span(i);
        
        // 注意：目前由于 TextBlockItem 无法访问 StyleManager，我们只使用 span 的直接样式
        // 如果需要完整支持命名样式，需要通过某种方式（例如从 DocumentScene 或其他地方）传递 StyleManager
        CharacterStyle style = span.style();
        
        QTextCharFormat format;
        
        // 设置字体
        format.setFont(style.font());
        
        // 设置颜色
        if (style.textColor().isValid()) {
            format.setForeground(style.textColor());
        }
        
        if (style.backgroundColor().isValid()) {
            format.setBackground(style.backgroundColor());
        }
        
        // 设置字间距
        if (style.letterSpacing() != 0) {
            format.setFontLetterSpacing(style.letterSpacing());
        }
        
        cursor.insertText(span.text(), format);
    }
}

} // namespace QtWordEditor
