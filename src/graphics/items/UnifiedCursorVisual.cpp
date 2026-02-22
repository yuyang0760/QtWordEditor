#include "graphics/items/UnifiedCursorVisual.h"
#include <QPainter>

namespace QtWordEditor {

UnifiedCursorVisual::UnifiedCursorVisual(QGraphicsItem *parent)
    : QGraphicsItem(parent)
    , m_height(20.0)
    , m_visible(true)
    , m_blinkState(true)
{
    // ========== 创建闪烁定时器，500ms 闪烁一次 ==========
    m_blinkTimer = new QTimer(this);
    connect(m_blinkTimer, &QTimer::timeout, this, &UnifiedCursorVisual::blink);
    m_blinkTimer->start(500);
    
    // ========== 设置 Z 轴值，确保光标在最上层 ==========
    setZValue(100);
}

UnifiedCursorVisual::~UnifiedCursorVisual()
{
    // ========== 停止并清理闪烁定时器 ==========
    if (m_blinkTimer) {
        m_blinkTimer->stop();
    }
}

void UnifiedCursorVisual::setPosition(const QPointF &pos, qreal height)
{
    // ========== 设置光标位置（相对于父项） ==========
    setPos(pos);
    m_height = height;
    update();
}

void UnifiedCursorVisual::setHeight(qreal height)
{
    // ========== 只更新高度，不改变位置 ==========
    m_height = height;
    update();
}

void UnifiedCursorVisual::setVisible(bool visible)
{
    // ========== 设置可见性并更新绘制 ==========
    m_visible = visible;
    update();
}

bool UnifiedCursorVisual::isVisible() const
{
    return m_visible;
}

void UnifiedCursorVisual::startBlink()
{
    // ========== 开始闪烁 ==========
    if (m_blinkTimer) {
        m_blinkTimer->start(500);
    }
    m_blinkState = true;
    update();
}

void UnifiedCursorVisual::stopBlink()
{
    // ========== 停止闪烁，保持可见 ==========
    if (m_blinkTimer) {
        m_blinkTimer->stop();
    }
    m_blinkState = true;
    update();
}

QRectF UnifiedCursorVisual::boundingRect() const
{
    // ========== 光标是一个 2px 宽的竖线 ==========
    return QRectF(0, 0, 2, m_height);
}

void UnifiedCursorVisual::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    
    // ========== 如果不可见或当前闪烁状态为隐藏，则不绘制 ==========
    if (!m_visible || !m_blinkState) {
        return;
    }
    
    // ========== 绘制黑色光标（2px 宽） ==========
    painter->setPen(QPen(Qt::black, 2));
    painter->drawLine(QPointF(1, 0), QPointF(1, m_height));
}

void UnifiedCursorVisual::blink()
{
    // ========== 切换闪烁状态并更新绘制 ==========
    m_blinkState = !m_blinkState;
    update();
}

} // namespace QtWordEditor
