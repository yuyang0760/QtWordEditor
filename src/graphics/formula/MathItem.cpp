/**
 * @file MathItem.cpp
 * @brief 所有公式视图元素的基类实现
 */

#include "graphics/formula/MathItem.h"
#include "core/document/MathSpan.h"
#include "graphics/items/TextBlockItem.h"
#include <QDebug>

namespace QtWordEditor {

MathItem::MathItem(MathSpan *span, MathItem *parent)
    : QGraphicsItem(parent)
    , m_boundingRect()
    , m_baseline(0)
    , m_span(span)
{
    setFlag(QGraphicsItem::ItemIsSelectable, false);
    setFlag(QGraphicsItem::ItemIsFocusable, false);
    // 接受左键事件，允许点击定位光标
    setAcceptedMouseButtons(Qt::LeftButton);
}

void MathItem::updateLayout()
{
    // 默认实现：子类重写
}

qreal MathItem::baseline() const
{
    return m_baseline;
}

void MathItem::insertChild(int index, MathItem *item)
{
    Q_UNUSED(index);
    Q_UNUSED(item);
    qWarning() << "MathItem::insertChild: Not a container type";
}

void MathItem::appendChild(MathItem *item)
{
    Q_UNUSED(item);
    qWarning() << "MathItem::appendChild: Not a container type";
}

void MathItem::removeChild(MathItem *item)
{
    Q_UNUSED(item);
    qWarning() << "MathItem::removeChild: Not a container type";
}

QList<MathItem*> MathItem::children() const
{
    return QList<MathItem*>();
}

int MathItem::childCount() const
{
    return 0;
}

MathItem *MathItem::childAt(int index) const
{
    Q_UNUSED(index);
    return nullptr;
}

int MathItem::indexOfChild(MathItem *child) const
{
    Q_UNUSED(child);
    return -1;
}

MathSpan *MathItem::mathSpan() const
{
    return m_span;
}

MathItem *MathItem::parentMathItem() const
{
    return dynamic_cast<MathItem*>(parentItem());
}

void MathItem::notifyParentLayoutChanged()
{
    qDebug() << "[MathItem::notifyParentLayoutChanged] 开始, this=" << this;
    
    MathItem *parent = parentMathItem();
    if (parent) {
        qDebug() << "[MathItem::notifyParentLayoutChanged] 有父 MathItem，调用父项 updateLayout()";
        parent->updateLayout();
    } else {
        qDebug() << "[MathItem::notifyParentLayoutChanged] 没有父 MathItem，寻找 TextBlockItem...";
        // 没有父 MathItem，说明这是根 MathItem，需要通知 TextBlockItem 更新布局
        QGraphicsItem *p = parentItem();
        while (p) {
            qDebug() << "[MathItem::notifyParentLayoutChanged] 检查 QGraphicsItem: type=" << p->type();
            // 检查是否是 TextBlockItem（Type = UserType + 1001）
            if (p->type() == QGraphicsItem::UserType + 1001) {
                qDebug() << "[MathItem::notifyParentLayoutChanged] 找到了 TextBlockItem，调用 safeUpdateLayout()";
                // 找到了 TextBlockItem，调用 safeUpdateLayout()
                TextBlockItem *textBlockItem = static_cast<TextBlockItem*>(p);
                textBlockItem->safeUpdateLayout();
                return;
            }
            p = p->parentItem();
        }
        qDebug() << "[MathItem::notifyParentLayoutChanged] 没有找到 TextBlockItem";
    }
}

void MathItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mousePressEvent(event);
    setFocus();
}

void MathItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mouseMoveEvent(event);
}

void MathItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mouseReleaseEvent(event);
}

void MathItem::keyPressEvent(QKeyEvent *event)
{
    QGraphicsItem::keyPressEvent(event);
}

int MathItem::hitTestRegion(const QPointF &localPos) const
{
    Q_UNUSED(localPos);
    // 默认实现：非容器类型返回 -1
    return -1;
}

} // namespace QtWordEditor
