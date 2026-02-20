#include "graphics/items/TextBlockItem.h"
#include "core/document/ParagraphBlock.h"
#include "core/document/ParagraphStyle.h"
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
    QFont font;  // 使用系统默认字体
    font.setPointSize(Constants::DefaultFontSize);
    m_textItem->setFont(font);
    
    // 内部 m_textItem 相对于 TextBlockItem 的位置是 (0,0)
    m_textItem->setPos(0, 0);
    // qDebug() << "TextBlockItem::initializeTextItem - 文本项位置:" << m_textItem->pos();
    
    // 应用段落缩进
    applyParagraphIndent();
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
    ParagraphBlock *para = qobject_cast<ParagraphBlock*>(m_block);
    qreal leftIndent = 0;
    qreal rightIndent = 0;
    
    // 如果有段落块，获取缩进值
    if (para) {
        ParagraphStyle style = para->paragraphStyle();
        leftIndent = style.leftIndent();
        rightIndent = style.rightIndent();
    }
    
    QRectF textRect = m_textItem->boundingRect();
    
    // 整体宽度 = 左缩进 + 文本宽度 + 右缩进
    qreal totalWidth = leftIndent + textRect.width() + rightIndent;
    
    setRect(0, 0, totalWidth, textRect.height());
}

void TextBlockItem::updateBlock()
{
    ParagraphBlock *para = qobject_cast<ParagraphBlock*>(m_block);
    if (!para)
        return;
    
    applyRichTextFromBlock();
    applyParagraphIndent();  // 应用段落缩进
    updateBoundingRect();
}

void TextBlockItem::applyRichTextFromBlock()
{
    ParagraphBlock *para = qobject_cast<ParagraphBlock*>(m_block);
    if (!para)
        return;
    
    qDebug() << "TextBlockItem::applyRichTextFromBlock - 开始应用，块ID:" << para->blockId();
    qDebug() << "  spanCount:" << para->spanCount();
    
    // 获取段落样式
    ParagraphStyle paraStyle = para->paragraphStyle();
    qreal firstLineIndent = paraStyle.firstLineIndent();
    ParagraphAlignment alignment = paraStyle.alignment();
    
    // 构建 HTML 字符串，给每个 Span 添加边框
    QString html;
    QList<QString> borderColors = {
        "#ff6666",  // 浅红色
        "#66ff66",  // 浅绿色
        "#e6e89c"   // 浅蓝色
    };
    
    // 添加外层 div，应用首行缩进和对齐方式
    QString divStyleString;
    QStringList styleList;
    
    // 应用首行缩进
    if (!qFuzzyIsNull(firstLineIndent)) {
        styleList << QString("text-indent: %1pt").arg(firstLineIndent);
    }
    
    // 应用对齐方式
    switch (alignment) {
        case ParagraphAlignment::AlignLeft:
            styleList << "text-align: left";
            break;
        case ParagraphAlignment::AlignCenter:
            styleList << "text-align: center";
            break;
        case ParagraphAlignment::AlignRight:
            styleList << "text-align: right";
            break;
        case ParagraphAlignment::AlignJustify:
            styleList << "text-align: justify";
            break;
        case ParagraphAlignment::AlignDistributed:
            // 分散对齐在 CSS 中使用 text-align: justify 并配合 text-justify: distribute
            styleList << "text-align: justify";
            styleList << "text-justify: distribute";
            break;
    }
    
    divStyleString = styleList.join("; ");
    html += QString("<div style=\"%1\">").arg(divStyleString);
    
    for (int i = 0; i < para->spanCount(); ++i) {
        Span span = para->span(i);
        CharacterStyle style = span.style();
        
        qDebug() << "  处理 span " << i << ": text=[" << span.text() << "], 加粗:" << style.bold();
        
        QString escapedText = span.text().toHtmlEscaped();
        QString color = borderColors[i % borderColors.size()];
        
        // 构建带边框的 span，同时添加字体和字号样式
        QString styleString = QString("border: 2px solid %1; background-color: %1; padding: 0px 2px;").arg(color);
        
        // 添加字体族
        if (!style.fontFamily().isEmpty()) {
            styleString += QString(" font-family: '%1';").arg(style.fontFamily());
        }
        
        // 添加字号
        if (style.fontSize() > 0) {
            styleString += QString(" font-size: %1pt;").arg(style.fontSize());
        }
        
        html += QString("<span style=\"%1\">").arg(styleString);
        
        // 添加粗体标记
        if (style.bold()) {
            html += "<b>";
        }
        
        // 添加斜体标记
        if (style.italic()) {
            html += "<i>";
        }
        
        // 添加下划线标记
        if (style.underline()) {
            html += "<u>";
        }
        
        html += escapedText;
        
        // 关闭标记
        if (style.underline()) {
            html += "</u>";
        }
        if (style.italic()) {
            html += "</i>";
        }
        if (style.bold()) {
            html += "</b>";
        }
        
        html += "</span>";
    }
    
    // 关闭外层 div
    html += "</div>";
    
    qDebug() << "  生成的 HTML:" << html;
    
    // 使用 HTML 设置文本
    m_textItem->setHtml(html);
    
    qDebug() << "TextBlockItem::applyRichTextFromBlock - 完成";
}

void TextBlockItem::applyParagraphIndent()
{
    ParagraphBlock *para = qobject_cast<ParagraphBlock*>(m_block);
    if (!para) {
        return;
    }
    
    // 获取段落样式中的缩进值
    ParagraphStyle style = para->paragraphStyle();
    qreal leftIndent = style.leftIndent();
    qreal rightIndent = style.rightIndent();
    
    // 1. 计算文本实际可用宽度 = 页面内容宽度 - 左缩进 - 右缩进
    qreal availableWidth = m_textWidth - leftIndent - rightIndent;
    
    // 最小宽度保护，防止缩进过大导致宽度为负
    if (availableWidth < 10.0) {
        availableWidth = 10.0;
    }
    
    // 2. 设置文本宽度
    m_textItem->setTextWidth(availableWidth);
    
    // 3. 调整文本项位置（向右偏移左缩进值）
    m_textItem->setPos(leftIndent, 0);
}

} // namespace QtWordEditor
