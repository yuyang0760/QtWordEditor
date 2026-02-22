#include "graphics/items/TextFragment.h"
#include <QPainter>
#include <QFontMetricsF>
#include <QDebug>

namespace QtWordEditor {

TextFragment::TextFragment(const QString &text, const CharacterStyle &style, QGraphicsItem *parent)
    : QGraphicsItem(parent)
    , m_text(text)
    , m_style(style)
    , m_textColor(Qt::black)
    , m_boundingRect(0, 0, 0, 0)
    , m_baseline(0)
{
    // 从样式创建字体
    m_font = createFontFromStyle(style);
    
    // 计算初始尺寸
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
        prepareGeometryChange();
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
    prepareGeometryChange();
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

QPointF TextFragment::cursorPosition(int charOffset) const
{
    // 使用 QFontMetricsF 计算指定偏移处的水平位置
    QFontMetricsF fm(m_font);
    QString textToOffset = m_text.left(qBound(0, charOffset, m_text.length()));
    qreal x = fm.horizontalAdvance(textToOffset);
    
    // y 坐标是基线位置
    return QPointF(x, m_baseline);
}

qreal TextFragment::cursorHeight() const
{
    // 光标高度是字体高度
    QFontMetricsF fm(m_font);
    return fm.height();
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
    
    // 绘制文本，基线对齐
    // 注意：m_baseline是从顶部到基线的距离，QPainter的drawText使用y坐标作为基线位置
    painter->drawText(QPointF(0, m_baseline), m_text);
}

void TextFragment::calculateSize()
{
    // 使用QFontMetricsF计算文本尺寸
    QFontMetricsF fm(m_font);
    
    // 计算文本宽度
    qreal textWidth = fm.horizontalAdvance(m_text);
    
    // 计算文本高度（从顶部到底部）
    qreal textHeight = fm.height();
    
    // 计算基线位置（从顶部到基线的距离）
    m_baseline = fm.ascent();
    
    // 设置边界矩形
    m_boundingRect = QRectF(0, 0, textWidth, textHeight);
    
    qDebug() << "TextFragment::calculateSize - text:" << m_text 
             << "width:" << textWidth << "height:" << textHeight 
             << "baseline:" << m_baseline;
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
        font.setPointSizeF(style.fontSize());
    }
    
    // 设置粗体
    font.setBold(style.bold());
    
    // 设置斜体
    font.setItalic(style.italic());
    
    // 设置下划线
    font.setUnderline(style.underline());
    
    return font;
}

} // namespace QtWordEditor
