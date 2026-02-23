#include "graphics/items/TextFragment.h"
#include <QPainter>
#include <QFontMetricsF>
#include "core/document/CharacterStyle.h"

namespace QtWordEditor {

TextFragment::TextFragment(const QString &text, const CharacterStyle &style, QGraphicsItem *parent)
    : QGraphicsItem(parent)
    , m_text(text)
    , m_style(style)
    , m_font(createFontFromStyle(style))
    , m_textColor(style.textColor())
    , m_baseline(0.0)
{
    // 计算文本尺寸
    calculateSize();
}

TextFragment::~TextFragment()
{
}

QString TextFragment::text() const
{
    return m_text;
}

void TextFragment::setText(const QString &text)
{
    if (m_text != text) {
        m_text = text;
        calculateSize();
        update();
    }
}

CharacterStyle TextFragment::style() const
{
    return m_style;
}

void TextFragment::setStyle(const CharacterStyle &style)
{
    if (!(m_style == style)) {
        m_style = style;
        m_font = createFontFromStyle(style);
        m_textColor = style.textColor();
        calculateSize();
        update();
    }
}

qreal TextFragment::width() const
{
    return m_boundingRect.width();
}

qreal TextFragment::height() const
{
    return m_boundingRect.height();
}

qreal TextFragment::baseline() const
{
    return m_baseline;
}

void TextFragment::calculateSize()
{
    QFontMetricsF fm(m_font);
    qreal textWidth = fm.horizontalAdvance(m_text);
    qreal textHeight = fm.height();
    m_baseline = fm.ascent();
    m_boundingRect = QRectF(0, 0, textWidth, textHeight);
}

QPointF TextFragment::cursorPosition(int charOffset) const
{
    // 确保偏移量在有效范围内
    if (charOffset < 0) {
        charOffset = 0;
    }
    if (charOffset > m_text.length()) {
        charOffset = m_text.length();
    }
    
    // 计算光标位置
    QFontMetricsF fm(m_font);
    QString textBeforeCursor = m_text.left(charOffset);
    qreal x = fm.horizontalAdvance(textBeforeCursor);
    
    // y 坐标为 0（文本顶部位置），而不是 m_baseline！
    return QPointF(x, 0);
}

qreal TextFragment::cursorHeight() const
{
    QFontMetricsF fm(m_font);
    return fm.height();
}

QRectF TextFragment::boundingRect() const
{
    return m_boundingRect;
}

void TextFragment::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    
    // 设置画笔颜色
    painter->setPen(m_textColor);
    painter->setFont(m_font);
    
    // 绘制文本，基线对齐（y坐标从 m_baseline 开始）
    painter->drawText(QPointF(0, m_baseline), m_text);
}

QFont TextFragment::createFontFromStyle(const CharacterStyle &style) const
{
    QFont font;
    font.setFamily(style.fontFamily());
    font.setPointSizeF(style.fontSize());
    font.setBold(style.bold());
    font.setItalic(style.italic());
    font.setUnderline(style.underline());
    return font;
}

} // namespace QtWordEditor
