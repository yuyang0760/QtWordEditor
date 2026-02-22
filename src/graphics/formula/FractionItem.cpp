/**
 * @file FractionItem.cpp
 * @brief 分数视图类实现
 */

#include "graphics/formula/FractionItem.h"
#include "core/document/math/FractionMathSpan.h"
#include <QPainter>

namespace QtWordEditor {

FractionItem::FractionItem(FractionMathSpan *span, MathItem *parent)
    : MathItem(span, parent)
    , m_numeratorItem(nullptr)
    , m_denominatorItem(nullptr)
    , m_numPos()
    , m_denPos()
    , m_lineY(0)
    , m_lineThickness(1.0)
    , m_padding(2.0)
{
}

FractionItem::~FractionItem()
{
    // 子元素由QGraphicsItem的父子关系管理
}

void FractionItem::updateLayout()
{
    FractionMathSpan *fracSpan = fractionSpan();
    if (!fracSpan) {
        return;
    }

    qreal totalWidth = 0;
    qreal numHeight = 0;
    qreal denHeight = 0;
    qreal numBaseline = 0;
    qreal denBaseline = 0;

    // 先更新分子和分母的布局
    if (m_numeratorItem) {
        m_numeratorItem->updateLayout();
        QRectF numRect = m_numeratorItem->boundingRect();
        numHeight = numRect.height();
        numBaseline = m_numeratorItem->baseline();
        if (numRect.width() > totalWidth) {
            totalWidth = numRect.width();
        }
    }

    if (m_denominatorItem) {
        m_denominatorItem->updateLayout();
        QRectF denRect = m_denominatorItem->boundingRect();
        denHeight = denRect.height();
        denBaseline = m_denominatorItem->baseline();
        if (denRect.width() > totalWidth) {
            totalWidth = denRect.width();
        }
    }

    // 添加左右padding
    totalWidth += 2 * m_padding;

    // 计算位置
    qreal totalHeight = numHeight + m_lineThickness + denHeight + 3 * m_padding;

    // 分子居中
    qreal numX = m_padding;
    if (m_numeratorItem) {
        qreal numWidth = m_numeratorItem->boundingRect().width();
        numX += (totalWidth - 2 * m_padding - numWidth) / 2.0;
    }
    m_numPos = QPointF(numX, m_padding);

    // 分数线位置
    m_lineY = m_padding + numHeight + m_padding;

    // 分母居中
    qreal denX = m_padding;
    if (m_denominatorItem) {
        qreal denWidth = m_denominatorItem->boundingRect().width();
        denX += (totalWidth - 2 * m_padding - denWidth) / 2.0;
    }
    m_denPos = QPointF(denX, m_lineY + m_lineThickness + m_padding);

    // 设置子元素位置
    if (m_numeratorItem) {
        m_numeratorItem->setPos(m_numPos);
    }
    if (m_denominatorItem) {
        m_denominatorItem->setPos(m_denPos);
    }

    m_boundingRect = QRectF(0, 0, totalWidth, totalHeight);
    m_baseline = m_lineY;  // 基线在分数线上
}

qreal FractionItem::baseline() const
{
    return m_baseline;
}

QRectF FractionItem::boundingRect() const
{
    return m_boundingRect;
}

void FractionItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    // 绘制分数线
    painter->save();
    painter->setPen(QPen(Qt::black, m_lineThickness));
    
    qreal lineX1 = m_padding;
    qreal lineX2 = m_boundingRect.width() - m_padding;
    painter->drawLine(QPointF(lineX1, m_lineY), QPointF(lineX2, m_lineY));
    
    painter->restore();
    
    // 子元素由QGraphicsItem自动绘制
}

QList<MathItem*> FractionItem::children() const
{
    QList<MathItem*> list;
    if (m_numeratorItem) {
        list.append(m_numeratorItem);
    }
    if (m_denominatorItem) {
        list.append(m_denominatorItem);
    }
    return list;
}

int FractionItem::childCount() const
{
    int count = 0;
    if (m_numeratorItem) count++;
    if (m_denominatorItem) count++;
    return count;
}

MathItem *FractionItem::childAt(int index) const
{
    if (index == 0) return m_numeratorItem;
    if (index == 1) return m_denominatorItem;
    return nullptr;
}

FractionMathSpan *FractionItem::fractionSpan() const
{
    return static_cast<FractionMathSpan*>(m_span);
}

MathItem *FractionItem::numeratorItem() const
{
    return m_numeratorItem;
}

void FractionItem::setNumeratorItem(MathItem *item)
{
    qDebug() << "[FractionItem::setNumeratorItem] 开始, item=" << item;
    if (m_numeratorItem != item) {
        qDebug() << "[FractionItem::setNumeratorItem] 分子需要更新";
        if (m_numeratorItem) {
            qDebug() << "[FractionItem::setNumeratorItem] 移除旧分子的 parent";
            m_numeratorItem->setParentItem(nullptr);
        }
        m_numeratorItem = item;
        if (m_numeratorItem) {
            qDebug() << "[FractionItem::setNumeratorItem] 设置新分子的 parent";
            m_numeratorItem->setParentItem(this);
        }
        qDebug() << "[FractionItem::setNumeratorItem] 准备调用 updateLayout()";
        updateLayout();
        qDebug() << "[FractionItem::setNumeratorItem] updateLayout() 完成";
    }
    qDebug() << "[FractionItem::setNumeratorItem] 结束";
}

MathItem *FractionItem::denominatorItem() const
{
    return m_denominatorItem;
}

void FractionItem::setDenominatorItem(MathItem *item)
{
    qDebug() << "[FractionItem::setDenominatorItem] 开始, item=" << item;
    if (m_denominatorItem != item) {
        qDebug() << "[FractionItem::setDenominatorItem] 分母需要更新";
        if (m_denominatorItem) {
            qDebug() << "[FractionItem::setDenominatorItem] 移除旧分母的 parent";
            m_denominatorItem->setParentItem(nullptr);
        }
        m_denominatorItem = item;
        if (m_denominatorItem) {
            qDebug() << "[FractionItem::setDenominatorItem] 设置新分母的 parent";
            m_denominatorItem->setParentItem(this);
        }
        qDebug() << "[FractionItem::setDenominatorItem] 准备调用 updateLayout()";
        updateLayout();
        qDebug() << "[FractionItem::setDenominatorItem] updateLayout() 完成";
    }
    qDebug() << "[FractionItem::setDenominatorItem] 结束";
}

int FractionItem::hitTestRegion(const QPointF &localPos) const
{
    qDebug() << "[FractionItem::hitTestRegion] localPos=" << localPos;
    
    // 判断是否在分子区域
    if (m_numeratorItem) {
        QRectF numRect = m_numeratorItem->boundingRect();
        numRect.translate(m_numPos);
        if (numRect.contains(localPos)) {
            qDebug() << "  在分子区域";
            return 0;
        }
    }
    
    // 判断是否在分母区域
    if (m_denominatorItem) {
        QRectF denRect = m_denominatorItem->boundingRect();
        denRect.translate(m_denPos);
        if (denRect.contains(localPos)) {
            qDebug() << "  在分母区域";
            return 1;
        }
    }
    
    // 如果都不在，可能是点击了分数线
    qDebug() << "  在其他区域";
    return -1;
}

QPointF FractionItem::numeratorPos() const
{
    return m_numPos;
}

QPointF FractionItem::denominatorPos() const
{
    return m_denPos;
}

} // namespace QtWordEditor
