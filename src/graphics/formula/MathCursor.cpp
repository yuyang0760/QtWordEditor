/**
 * @file MathCursor.cpp
 * @brief 数学公式光标类实现
 */

#include "graphics/formula/MathCursor.h"
#include "graphics/formula/RowContainerItem.h"
#include <QPainter>
#include <QTimer>

namespace QtWordEditor {

MathCursor::MathCursor(QGraphicsItem *parent)
    : QGraphicsItem(parent),
      m_currentContainer(nullptr),
      m_position(0),
      m_height(20.0),
      m_visible(true),
      m_blinkState(true)
{
    // 创建闪烁定时器，500ms 闪烁一次
    m_blinkTimer = new QTimer(this);
    connect(m_blinkTimer, &QTimer::timeout, this, &MathCursor::blink);
    m_blinkTimer->start(500);
    
    setZValue(100);  // 确保光标在最上层
}

MathCursor::~MathCursor()
{
    if (m_blinkTimer) {
        m_blinkTimer->stop();
    }
}

void MathCursor::setPosition(RowContainerItem *container, int position)
{
    m_currentContainer = container;
    m_position = position;
    updateVisualPosition();
    update();
}

RowContainerItem *MathCursor::currentContainer() const
{
    return m_currentContainer;
}

int MathCursor::position() const
{
    return m_position;
}

void MathCursor::moveLeft()
{
    if (m_currentContainer && m_position > 0) {
        m_position--;
        updateVisualPosition();
        update();
    }
}

void MathCursor::moveRight()
{
    if (m_currentContainer) {
        int maxPos = m_currentContainer->childCount();
        if (m_position < maxPos) {
            m_position++;
            updateVisualPosition();
            update();
        }
    }
}

void MathCursor::moveUp()
{
    // 向上移动（暂时不实现，用于分数的分子分母切换）
}

void MathCursor::moveDown()
{
    // 向下移动（暂时不实现，用于分数的分子分母切换）
}

void MathCursor::moveToParent()
{
    // 移动到父容器（暂时不实现）
}

void MathCursor::setVisible(bool visible)
{
    m_visible = visible;
    update();
}

void MathCursor::setHeight(qreal height)
{
    m_height = height;
    update();
}

void MathCursor::updateVisualPosition()
{
    if (m_currentContainer) {
        // 使用 RowContainerItem 的 cursorXAt() 方法获取正确的 X 坐标
        qreal x = m_currentContainer->cursorXAt(m_position);
        qreal y = 0;
        
        // 计算 Y 坐标，使光标与基线对齐
        qreal baseline = m_currentContainer->baseline();
        qreal topY = 0;  // 假设基线相对于容器顶部
        
        // 光标从基线向上延伸 m_height 高度
        y = baseline - m_height;
        
        setPos(QPointF(x, y));
    }
}

QRectF MathCursor::boundingRect() const
{
    // 光标是一个 2px 宽的竖线
    return QRectF(0, 0, 2, m_height);
}

void MathCursor::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    
    if (!m_visible || !m_blinkState) {
        return;
    }
    
    // 绘制黑色光标
    painter->setPen(QPen(Qt::black, 2));
    painter->drawLine(QPointF(1, 0), QPointF(1, m_height));
}

void MathCursor::blink()
{
    m_blinkState = !m_blinkState;
    update();
}

} // namespace QtWordEditor
