/**
 * @file GenericMathItem.cpp
 * @brief 通用公式容器视图类（简化版）实现
 */

#include "graphics/formula/GenericMathItem.h"
#include "core/document/math/GenericMathSpan.h"
#include "graphics/factory/MathItemFactory.h"
#include <QPainter>
#include <QDebug>

namespace QtWordEditor {

GenericMathItem::GenericMathItem(GenericMathSpan *span, MathItem *parent)
    : MathItem(span, parent)
    , m_layoutEngine(new TextBlockLayoutEngine())
    , m_mathItems()
    , m_boundingRect()
{
    // 配置布局引擎为 NoWrap 模式
    m_layoutEngine->setWrapMode(TextBlockLayoutEngine::WrapMode::NoWrap);
    m_layoutEngine->setAvailableWidth(100000.0); // 足够大的宽度，不换行
    
    // 初始化 MathItem 子项
    updateMathItems();
}

GenericMathItem::~GenericMathItem()
{
    delete m_layoutEngine;
    clearMathItems();
}

void GenericMathItem::updateMathItems()
{
    // 清除旧的 MathItem
    clearMathItems();
    
    QList<InlineSpan*> spans = getSpans();
    
    // ========== 第一步：先创建所有 MathItem，获取真实尺寸 ==========
    QHash<InlineSpan*, MathItem*> mathItemMap;
    QHash<InlineSpan*, QSizeF> mathSizeMap;
    QHash<InlineSpan*, qreal> mathBaselineMap;
    
    for (InlineSpan *span : spans) {
        if (span->type() == InlineSpan::Math) {
            MathSpan *mathSpan = static_cast<MathSpan*>(span);
            MathItem *mathItem = MathItemFactory::createMathItem(mathSpan, this);
            if (mathItem) {
                mathItem->setParentItem(this);
                mathItem->setVisible(false); // 先隐藏，等布局后再显示
                m_mathItems.append(mathItem);
                mathItemMap.insert(span, mathItem);
                mathSizeMap.insert(span, mathItem->boundingRect().size());
                mathBaselineMap.insert(span, mathItem->baseline());
            }
        }
    }
    // ===============================================================
    
    // ========== 第二步：执行布局（现在可以使用真实尺寸） ==========
    if (mathSizeMap.isEmpty()) {
        performLayout();
    } else {
        performLayoutWithMathSizes(mathSizeMap, mathBaselineMap);
    }
    // ===============================================================
    
    // ========== 第三步：设置 MathItem 的位置并显示 ==========
    const QList<TextBlockLayoutEngine::LayoutItem> &items = m_layoutEngine->layoutItems();
    for (const TextBlockLayoutEngine::LayoutItem &item : items) {
        if (item.inlineSpan && item.inlineSpan->type() == InlineSpan::Math) {
            MathItem *mathItem = mathItemMap.value(item.inlineSpan, nullptr);
            if (mathItem) {
                mathItem->setPos(item.position);
                mathItem->setVisible(true);
            }
        }
    }
    // ===============================================================
}

void GenericMathItem::clearMathItems()
{
    qDeleteAll(m_mathItems);
    m_mathItems.clear();
}

QList<InlineSpan*> GenericMathItem::getSpans() const
{
    GenericMathSpan *span = genericSpan();
    if (!span) {
        return QList<InlineSpan*>();
    }
    return span->spans();
}

void GenericMathItem::performLayout()
{
    QList<InlineSpan*> spans = getSpans();
    
    // 使用布局引擎
    m_layoutEngine->layout(spans);
    
    // 更新边界矩形
    m_boundingRect = QRectF(0, 0, m_layoutEngine->totalWidth(), m_layoutEngine->totalHeight());
}

void GenericMathItem::performLayoutWithMathSizes(const QHash<InlineSpan*, QSizeF> &mathSizeMap, 
                                                   const QHash<InlineSpan*, qreal> &mathBaselineMap)
{
    QList<InlineSpan*> spans = getSpans();
    
    // 使用布局引擎
    m_layoutEngine->layout(spans, mathSizeMap, mathBaselineMap);
    
    // 更新边界矩形
    m_boundingRect = QRectF(0, 0, m_layoutEngine->totalWidth(), m_layoutEngine->totalHeight());
}

void GenericMathItem::updateLayout()
{
    // 更新 MathItem 的布局
    for (MathItem *mathItem : m_mathItems) {
        mathItem->updateLayout();
    }
    
    // 重新创建 MathItem 并布局
    updateMathItems();
    
    // 设置基线（使用第一行的基线）
    if (!m_layoutEngine->lines().isEmpty()) {
        m_baseline = m_layoutEngine->lines().first().maxBaseline;
    } else {
        m_baseline = 0;
    }
    
    notifyParentLayoutChanged();
}

qreal GenericMathItem::baseline() const
{
    return m_baseline;
}

QRectF GenericMathItem::boundingRect() const
{
    return m_boundingRect;
}

void GenericMathItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    
    // 绘制文本
    const QList<TextBlockLayoutEngine::LayoutItem> &items = m_layoutEngine->layoutItems();
    for (int i = 0; i < items.size(); ++i) {
        const TextBlockLayoutEngine::LayoutItem &item = items[i];
        if (item.inlineSpan && item.inlineSpan->type() == InlineSpan::Text) {
            // 绘制文本
            painter->setFont(item.font);
            painter->setPen(Qt::black);
            
            // 计算文本位置（基线对齐）
            QPointF textPos = item.position;
            textPos.ry() += item.ascent;
            painter->drawText(textPos, item.text);
        }
        // MathItem 由 QGraphicsItem 自动绘制
    }
}

QList<MathItem*> GenericMathItem::children() const
{
    return m_mathItems;
}

int GenericMathItem::childCount() const
{
    return m_mathItems.size();
}

MathItem *GenericMathItem::childAt(int index) const
{
    if (index >= 0 && index < m_mathItems.size()) {
        return m_mathItems.at(index);
    }
    return nullptr;
}

int GenericMathItem::indexOfChild(MathItem *child) const
{
    return m_mathItems.indexOf(child);
}

GenericMathSpan *GenericMathItem::genericSpan() const
{
    return static_cast<GenericMathSpan*>(m_span);
}

GenericMathItem::CursorVisualInfo GenericMathItem::cursorPositionAt(int globalOffset) const
{
    CursorVisualInfo info;
    
    QList<InlineSpan*> spans = getSpans();
    TextBlockLayoutEngine::CursorVisualResult result = m_layoutEngine->cursorPositionAt(globalOffset, spans);
    
    info.position = result.position;
    info.height = result.height;
    
    return info;
}

int GenericMathItem::hitTest(const QPointF &localPos) const
{
    QList<InlineSpan*> spans = getSpans();
    TextBlockLayoutEngine::CursorHitResult result = m_layoutEngine->hitTest(localPos, spans);
    return result.globalOffset;
}

} // namespace QtWordEditor
