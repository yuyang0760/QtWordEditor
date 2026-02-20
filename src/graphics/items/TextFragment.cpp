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
    , m_availableWidth(100000)  // 默认值，足够大不换行
{
    // 从 CharacterStyle 创建字体
    m_font = createFontFromStyle(style);
    
    // 设置 QTextLayout
    m_textLayout.setText(m_text);
    m_textLayout.setFont(m_font);
    
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
        m_textLayout.setText(m_text);
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
    m_textLayout.setFont(m_font);
    calculateSize();
    update();
}

QFont TextFragment::font() const
{
    return m_font;
}

void TextFragment::setAvailableWidth(qreal width)
{
    if (m_availableWidth != width) {
        m_availableWidth = width;
        calculateSize();
        update();
    }
}

qreal TextFragment::availableWidth() const
{
    return m_availableWidth;
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
    if (m_text.isEmpty()) {
        return 0;
    }
    
    qreal x = localPos.x();
    qreal y = localPos.y();
    
    // 找到点击的行
    for (int i = 0; i < m_textLines.size(); ++i) {
        const QTextLine &line = m_textLines[i];
        qreal lineY = line.y();
        qreal lineHeight = line.height();
        
        if (y >= lineY && y <= lineY + lineHeight) {
            // 这一行被点击了
            qreal localX = x;
            int offset = line.xToCursor(localX);
            return line.textStart() + offset;
        }
    }
    
    // 备用：如果没找到，就返回 0
    return 0;
}

QPointF TextFragment::cursorPositionAt(int offset) const
{
    // 确保 offset 在有效范围内
    offset = qBound(0, offset, m_text.length());
    
    qDebug() << "TextFragment::cursorPositionAt - offset:" << offset 
             << "text:" << m_text.left(10) << "...";
    
    QFontMetricsF fm(m_font);
    qDebug() << "  baseline:" << m_baseline 
             << "ascent:" << fm.ascent() 
             << "descent:" << fm.descent() 
             << "height:" << fm.height();
    
    // 找到包含这个 offset 的行
    for (const QTextLine &line : m_textLines) {
        int lineStart = line.textStart();
        int lineEnd = lineStart + line.textLength();
        
        if (offset >= lineStart && offset <= lineEnd) {
            qreal x = line.cursorToX(offset - lineStart);
            qreal y = line.y();  // Y 坐标是这一行的顶部
            
            qDebug() << "  返回位置: x=" << x << "y=" << y;
            
            return QPointF(x, y);
        }
    }
    
    // 备用：第一行
    if (!m_textLines.isEmpty()) {
        const QTextLine &line = m_textLines.first();
        qreal x = line.cursorToX(0);
        qreal y = line.y();
        return QPointF(x, y);
    }
    
    return QPointF(0, 0);
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
    
    // 绘制每一行
    // QTextLine 的 position 已经在 calculateSize() 中设置好了
    // 所以直接用 QPointF(0, 0) 作为偏移即可
    m_textLayout.draw(painter, QPointF(0, 0));
}

void TextFragment::calculateSize()
{
    m_textLayout.beginLayout();
    
    m_textLines.clear();
    qreal currentY = 0;
    
    while (true) {
        QTextLine line = m_textLayout.createLine();
        if (!line.isValid()) {
            break;
        }
        
        line.setLineWidth(m_availableWidth);
        line.setPosition(QPointF(0, currentY));
        m_textLines.append(line);
        
        currentY += line.height();
    }
    
    m_textLayout.endLayout();
    
    // 计算边界矩形
    if (!m_textLines.isEmpty()) {
        const QTextLine &lastLine = m_textLines.last();
        qreal maxWidth = 0;
        for (const QTextLine &line : m_textLines) {
            if (line.width() > maxWidth) {
                maxWidth = line.width();
            }
        }
        qreal totalHeight = lastLine.y() + lastLine.height();
        m_boundingRect = QRectF(0, 0, maxWidth, totalHeight);
        
        // 基线取第一行的 baseline
        const QTextLine &firstLine = m_textLines.first();
        m_baseline = firstLine.ascent();
    } else {
        m_boundingRect = QRectF(0, 0, 0, 0);
        m_baseline = 0;
    }
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
