#include "graphics/items/TextFragment.h"
#include <QPainter>
#include <QFontMetricsF>
#include <QDebug>

namespace QtWordEditor {

TextFragment::TextFragment(const QString &text, const CharacterStyle &style, QGraphicsItem *parent)
    : QGraphicsItem(parent)
    , m_text(text)
    , m_style(style)
    , m_textColor(style.textColor())
    , m_boundingRect(0, 0, 0, 0)
    , m_baseline(0)
{
    // 从 CharacterStyle 创建字体
    m_font = createFontFromStyle(style);
    
    // 计算文本尺寸
    calculateSize();
}

TextFragment::~TextFragment()
{
}

int TextFragment::type() const
{
    return Type;
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
    m_style = style;
    m_font = createFontFromStyle(style);
    m_textColor = style.textColor();
    calculateSize();
    update();
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

QRectF TextFragment::boundingRect() const
{
    return m_boundingRect;
}

void TextFragment::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    
    // 设置字体和颜色
    painter->setFont(m_font);
    painter->setPen(m_textColor);
    
    // 绘制文本（在基线上绘制）
    painter->drawText(QPointF(0, m_baseline), m_text);
}

void TextFragment::calculateSize()
{
    QFontMetricsF fm(m_font);
    
    // 计算宽度（使用 horizontalAdvance 更准确）
    qreal width = fm.horizontalAdvance(m_text);
    
    // 计算高度
    qreal height = fm.height();
    
    // 计算基线（ascent 是从顶部到基线的距离）
    m_baseline = fm.ascent();
    
    // 设置边界矩形
    m_boundingRect = QRectF(0, 0, width, height);
}

QFont TextFragment::createFontFromStyle(const CharacterStyle &style) const
{
    QFont font;
    
    // 设置字体族
    if (!style.fontFamily().isEmpty()) {
        font.setFamily(style.fontFamily());
    }
    
    // 设置字号
    if (style.fontSize() > 0) {
        font.setPointSize(style.fontSize());
    }
    
    // 设置粗体
    font.setBold(style.bold());
    
    // 设置斜体
    font.setItalic(style.italic());
    
    // 设置下划线
    font.setUnderline(style.underline());
    
    // 设置删除线
    font.setStrikeOut(style.strikeOut());
    
    // 设置字符间距
    if (!qFuzzyIsNull(style.letterSpacing())) {
        font.setLetterSpacing(QFont::AbsoluteSpacing, style.letterSpacing());
    }
    
    return font;
}

} // namespace QtWordEditor
