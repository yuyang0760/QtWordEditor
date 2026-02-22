/**
 * @file NumberItem.cpp
 * @brief 数字/变量视图类实现
 */

#include "graphics/formula/NumberItem.h"
#include "core/document/math/NumberMathSpan.h"
#include <QFontMetrics>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>

namespace QtWordEditor {

NumberItem::NumberItem(NumberMathSpan *span, MathItem *parent)
    : MathItem(span, parent)
    , m_font("Microsoft YaHei", 12)
    , m_selectionStart(0)
    , m_selectionEnd(0)
    , m_isSelecting(false)
{
    updateLayout();
}

NumberItem::~NumberItem()
{
}

void NumberItem::updateLayout()
{
    NumberMathSpan *numSpan = numberSpan();
    if (!numSpan) {
        return;
    }

    QFontMetrics fm(m_font);
    QString text = numSpan->text();
    
    if (text.isEmpty()) {
        text = " ";  // 空时显示一个占位符
    }

    qreal width = fm.horizontalAdvance(text);
    qreal height = fm.height();
    qreal ascent = fm.ascent();

    // ========== 添加一点边距，避免光标和文字紧贴边缘 ==========
    qreal margin = 2.0;
    
    m_boundingRect = QRectF(0, 0, width + margin * 2, height);
    m_baseline = ascent;
}

qreal NumberItem::baseline() const
{
    return m_baseline;
}

QRectF NumberItem::boundingRect() const
{
    return m_boundingRect;
}

NumberMathSpan *NumberItem::numberSpan() const
{
    return static_cast<NumberMathSpan*>(m_span);
}

qreal NumberItem::cursorXAt(int position) const
{
    NumberMathSpan *numSpan = numberSpan();
    if (!numSpan) {
        return 0;
    }

    // ========== 边距值 ==========
    qreal margin = 2.0;

    QString text = numSpan->text();
    if (position <= 0) {
        return margin;  // 光标在最左边时，也从边距开始
    }

    if (position >= text.length()) {
        return margin + QFontMetrics(m_font).horizontalAdvance(text);  // 光标在最右边时，在文本结束位置
    }

    // 计算到第 position 个字符的宽度
    QFontMetrics fm(m_font);
    QString subText = text.left(position);
    return margin + fm.horizontalAdvance(subText);
}

int NumberItem::textLength() const
{
    NumberMathSpan *numSpan = numberSpan();
    if (!numSpan) {
        return 0;
    }
    return numSpan->text().length();
}

int NumberItem::hitTestX(qreal x) const
{
    NumberMathSpan *numSpan = numberSpan();
    if (!numSpan) {
        return 0;
    }

    // ========== 边距值 ==========
    qreal margin = 2.0;
    
    QString text = numSpan->text();
    if (text.isEmpty()) {
        return 0;
    }

    // ========== 边界情况处理 ==========
    // 点击在左边距区域，返回位置 0
    if (x <= margin) {
        return 0;
    }

    QFontMetrics fm(m_font);
    qreal textWidth = fm.horizontalAdvance(text);
    
    // 点击在右边距区域，返回文本末尾位置
    if (x >= margin + textWidth) {
        return text.length();
    }

    // ========== 正常情况：在文本区域内点击 ==========
    // 从第一个字符开始，找到最合适的位置（减去边距）
    qreal adjustedX = x - margin;
    for (int i = 0; i < text.length(); ++i) {
        QString subTextBefore = text.left(i);
        qreal charXBefore = fm.horizontalAdvance(subTextBefore);
        
        QString subTextAt = text.left(i + 1);
        qreal charXAt = fm.horizontalAdvance(subTextAt);
        
        // ========== 计算当前字符的中心位置 ==========
        qreal charCenterX = (charXBefore + charXAt) / 2.0;
        
        // ========== 如果点击位置在字符中心偏左，光标放在左边（i） ==========
        // ========== 如果点击位置在字符中心偏右，光标放在右边（i+1） ==========
        if (adjustedX < charCenterX) {
            return i;
        }
    }
    
    // 循环结束，说明点击位置在最后一个字符的右边
    return text.length();
}

// ========== 文本选择接口实现 ==========

int NumberItem::selectionStart() const
{
    return m_selectionStart;
}

int NumberItem::selectionEnd() const
{
    return m_selectionEnd;
}

void NumberItem::setSelection(int start, int end)
{
    NumberMathSpan *numSpan = numberSpan();
    if (!numSpan) {
        return;
    }
    
    int length = numSpan->text().length();
    
    // 确保位置在有效范围内
    m_selectionStart = qBound(0, start, length);
    m_selectionEnd = qBound(0, end, length);
    
    update();
}

void NumberItem::clearSelection()
{
    m_selectionStart = 0;
    m_selectionEnd = 0;
    update();
}

bool NumberItem::hasSelection() const
{
    return m_selectionStart != m_selectionEnd;
}

// ========== 选择操作的公共方法 ==========

void NumberItem::startSelectionAt(qreal localX)
{
    qDebug() << "[NumberItem::startSelectionAt] localX=" << localX;
    
    // 清除之前的选择
    clearSelection();
    
    // 获取点击位置
    int pos = hitTestX(localX);
    
    // 设置选择起始和结束位置
    m_selectionStart = pos;
    m_selectionEnd = pos;
    
    // 开始选择
    m_isSelecting = true;
    
    update();
}

void NumberItem::updateSelectionAt(qreal localX)
{
    if (!m_isSelecting) {
        return;
    }
    
    qDebug() << "[NumberItem::updateSelectionAt] localX=" << localX;
    
    // 获取当前位置
    int pos = hitTestX(localX);
    
    // 更新选择结束位置
    m_selectionEnd = pos;
    
    update();
}

void NumberItem::endSelectionAt(qreal localX)
{
    qDebug() << "[NumberItem::endSelectionAt] localX=" << localX;
    
    if (m_isSelecting) {
        // 获取当前位置
        int pos = hitTestX(localX);
        
        // 更新选择结束位置
        m_selectionEnd = pos;
        
        // 结束选择
        m_isSelecting = false;
        
        update();
    }
}

// ========== 重写 paint 方法，添加选择区域绘制 ==========

void NumberItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    NumberMathSpan *numSpan = numberSpan();
    if (!numSpan) return;

    painter->save();
    
    // ========== 先绘制选择区域 ==========
    if (hasSelection()) {
        qreal margin = 2.0;
        QFontMetrics fm(m_font);
        QString text = numSpan->text();
        
        int start = qMin(m_selectionStart, m_selectionEnd);
        int end = qMax(m_selectionStart, m_selectionEnd);
        
        if (start < end) {
            QString beforeText = text.left(start);
            QString selectedText = text.mid(start, end - start);
            
            qreal x1 = margin + fm.horizontalAdvance(beforeText);
            qreal x2 = margin + fm.horizontalAdvance(beforeText + selectedText);
            qreal height = m_boundingRect.height();
            
            // 绘制选择高亮（使用系统选择色）
            painter->fillRect(QRectF(x1, 0, x2 - x1, height), QColor(0, 120, 215, 50));
        }
    }
    
    // ========== 再绘制文本 ==========
    painter->setFont(m_font);
    painter->setPen(Qt::black);

    QString text = numSpan->text();
    if (text.isEmpty()) {
        text = " ";
    }

    // ========== 文本位置向右偏移边距，这样文字不会紧贴左边缘 ==========
    qreal margin = 2.0;
    painter->drawText(QPointF(margin, m_baseline), text);
    
    painter->restore();
}

} // namespace QtWordEditor
