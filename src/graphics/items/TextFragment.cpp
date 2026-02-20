#include "graphics/items/TextFragment.h"
#include <QPainter>
#include <QFontMetricsF>
#include <QDebug>
#include <QTextLayout>
#include <QTextLine>

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

QFont TextFragment::font() const
{
    return m_font;
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

int TextFragment::hitTest(const QPointF &localPos) const
{
    QFontMetricsF fm(m_font);
    
    if (m_text.isEmpty()) {
        return 0;
    }
    
    qreal x = localPos.x();
    
    // 使用 QTextLayout 来获得更准确的 hitTest
    QTextLayout textLayout(m_text, m_font);
    textLayout.beginLayout();
    QTextLine line = textLayout.createLine();
    line.setLineWidth(100000); // 足够长的宽度，不换行
    textLayout.endLayout();
    
    if (line.isValid()) {
        // 使用 xToCursor 获取准确位置
        int offset = line.xToCursor(x);
        return offset;
    }
    
    // 备用方案：逐个字符计算
    int bestOffset = 0;
    qreal minDistance = qAbs(x);
    
    for (int i = 0; i <= m_text.length(); ++i) {
        QString leftPart = m_text.left(i);
        qreal posX = fm.horizontalAdvance(leftPart);
        qreal distance = qAbs(x - posX);
        
        if (distance < minDistance) {
            minDistance = distance;
            bestOffset = i;
        }
    }
    
    return bestOffset;
}

QPointF TextFragment::cursorPositionAt(int offset) const
{
    QFontMetricsF fm(m_font);
    
    // 确保 offset 在有效范围内
    offset = qBound(0, offset, m_text.length());
    
    qDebug() << "TextFragment::cursorPositionAt - offset:" << offset 
             << "text:" << m_text.left(10) << "...";
    qDebug() << "  baseline:" << m_baseline 
             << "ascent:" << fm.ascent() 
             << "descent:" << fm.descent() 
             << "height:" << fm.height();
    
    // 使用 QTextLayout 来获得更准确的光标位置
    QTextLayout textLayout(m_text, m_font);
    textLayout.beginLayout();
    QTextLine line = textLayout.createLine();
    line.setLineWidth(100000); // 足够长的宽度，不换行
    textLayout.endLayout();
    
    qreal x = 0;
    if (line.isValid()) {
        x = line.cursorToX(offset);
    } else {
        QString leftPart = m_text.left(offset);
        x = fm.horizontalAdvance(leftPart);
    }
    
    // Y 坐标：文本顶部位置 = 0
    // 这样光标会从文本顶部开始，覆盖整个高度
    qreal y = 0;
    
    qDebug() << "  返回位置: x=" << x << "y=" << y;
    
    return QPointF(x, y);
}

qreal TextFragment::cursorHeight() const
{
    QFontMetricsF fm(m_font);
    qreal height = fm.height();
    qDebug() << "TextFragment::cursorHeight - height:" << height
             << "ascent:" << fm.ascent()
             << "descent:" << fm.descent();
    return height;
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
