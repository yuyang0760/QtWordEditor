/**
 * @file MathCursor.cpp
 * @brief 数学公式光标类实现
 */

#include "graphics/formula/MathCursor.h"
#include "graphics/formula/RowContainerItem.h"
#include "graphics/formula/NumberItem.h"
#include "graphics/formula/FractionItem.h"
#include <QPainter>
#include <QTimer>

namespace QtWordEditor {

MathCursor::MathCursor(QGraphicsItem *parent)
    : QGraphicsItem(parent),
      m_currentContainer(nullptr),
      m_currentNumberItem(nullptr),
      m_cursorMode(ContainerMode),
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
    m_cursorMode = ContainerMode;
    m_currentContainer = container;
    m_currentNumberItem = nullptr;
    m_position = position;
    
    if (container) {
        setParentItem(container);
    }
    
    updateVisualPosition();
    update();
}

void MathCursor::setPosition(NumberItem *numberItem, int position)
{
    m_cursorMode = NumberMode;
    m_currentNumberItem = numberItem;
    m_currentContainer = nullptr;
    m_position = position;
    
    if (numberItem) {
        setParentItem(numberItem);
    }
    
    updateVisualPosition();
    update();
}

RowContainerItem *MathCursor::currentContainer() const
{
    return m_currentContainer;
}

NumberItem *MathCursor::currentNumberItem() const
{
    return m_currentNumberItem;
}

MathCursor::CursorMode MathCursor::cursorMode() const
{
    return m_cursorMode;
}

int MathCursor::position() const
{
    return m_position;
}

void MathCursor::moveLeft()
{
    if (m_cursorMode == NumberMode && m_currentNumberItem) {
        if (m_position > 0) {
            m_position--;
            updateVisualPosition();
            update();
        }
    } else if (m_cursorMode == ContainerMode && m_currentContainer) {
        if (m_position > 0) {
            m_position--;
            updateVisualPosition();
            update();
        }
    }
}

void MathCursor::moveRight()
{
    if (m_cursorMode == NumberMode && m_currentNumberItem) {
        int maxPos = m_currentNumberItem->textLength();
        if (m_position < maxPos) {
            m_position++;
            updateVisualPosition();
            update();
        }
    } else if (m_cursorMode == ContainerMode && m_currentContainer) {
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
    // 检查是否在 NumberItem 模式下
    if (m_cursorMode != NumberMode || !m_currentNumberItem) {
        return;
    }
    
    // 查找父级是否是 FractionItem
    QGraphicsItem *parent = m_currentNumberItem->parentItem();
    while (parent) {
        FractionItem *fracItem = dynamic_cast<FractionItem*>(parent);
        if (fracItem) {
            // 检查当前是在分母还是分子
            if (m_currentNumberItem == fracItem->denominatorItem()) {
                // 当前在分母，移动到分子
                NumberItem *numItem = dynamic_cast<NumberItem*>(fracItem->numeratorItem());
                if (numItem) {
                    // 计算相同的字符位置（或边界）
                    int newPos = qMin(m_position, numItem->textLength());
                    setPosition(numItem, newPos);
                }
            }
            return;
        }
        parent = parent->parentItem();
    }
}

void MathCursor::moveDown()
{
    // 检查是否在 NumberItem 模式下
    if (m_cursorMode != NumberMode || !m_currentNumberItem) {
        return;
    }
    
    // 查找父级是否是 FractionItem
    QGraphicsItem *parent = m_currentNumberItem->parentItem();
    while (parent) {
        FractionItem *fracItem = dynamic_cast<FractionItem*>(parent);
        if (fracItem) {
            // 检查当前是在分子还是分母
            if (m_currentNumberItem == fracItem->numeratorItem()) {
                // 当前在分子，移动到分母
                NumberItem *denItem = dynamic_cast<NumberItem*>(fracItem->denominatorItem());
                if (denItem) {
                    // 计算相同的字符位置（或边界）
                    int newPos = qMin(m_position, denItem->textLength());
                    setPosition(denItem, newPos);
                }
            }
            return;
        }
        parent = parent->parentItem();
    }
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
    if (m_cursorMode == NumberMode && m_currentNumberItem) {
        // 数字模式：使用 NumberItem 的 cursorXAt() 方法
        qreal x = m_currentNumberItem->cursorXAt(m_position);
        qreal baseline = m_currentNumberItem->baseline();
        
        // 光标从基线向上延伸 m_height 高度
        qreal y = baseline - m_height;
        
        setPos(QPointF(x, y));
    } else if (m_cursorMode == ContainerMode && m_currentContainer) {
        // 容器模式：使用 RowContainerItem 的 cursorXAt() 方法
        qreal x = m_currentContainer->cursorXAt(m_position);
        qreal baseline = m_currentContainer->baseline();
        
        // 光标从基线向上延伸 m_height 高度
        qreal y = baseline - m_height;
        
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
