#include "graphics/items/CursorItem.h"
#include <QObject>
#include <QPen>
#include <QBrush>
#include <QDebug>

namespace QtWordEditor {

CursorItem::CursorItem(QGraphicsItem *parent)
    : QGraphicsLineItem(parent)
    , m_visible(true)
    , m_cursorEnabled(true)  // 默认为启用
{
    setPen(QPen(Qt::black, 1));
    m_blinkTimer.setInterval(500);
    QObject::connect(&m_blinkTimer, &QTimer::timeout, [this]() { toggleBlink(); });
    startBlink();
}

CursorItem::~CursorItem()
{
}

void CursorItem::setPosition(const QPointF &pos, qreal height)
{
    setLine(pos.x(), pos.y(), pos.x(), pos.y() + height);
}

void CursorItem::startBlink()
{
    m_blinkTimer.start();
}

void CursorItem::stopBlink()
{
    m_blinkTimer.stop();
    setVisible(true);
}

void CursorItem::toggleBlink()
{
    // 只有在光标启用时才闪烁
    if (m_cursorEnabled) {
        m_visible = !m_visible;
        setVisible(m_visible);
    }
}

// ========== 新增方法：设置光标可见性 ==========
void CursorItem::setCursorVisible(bool visible)
{
    m_cursorEnabled = visible;
    
    if (visible) {
        // 显示光标：恢复可见性并开始闪烁
        m_visible = true;
        setVisible(true);
        startBlink();
    } else {
        // 隐藏光标：停止闪烁并隐藏
        stopBlink();
        setVisible(false);
    }
}

} // namespace QtWordEditor