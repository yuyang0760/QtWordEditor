/**
 * @file GenericMathItem.cpp
 * @brief 通用公式容器视图类（简化版）实现
 * 
 * 该类使用 TextBlockLayoutEngine，类似 TextBlockItem 的设计，
 * 但配置为 NoWrap 模式，没有段落、缩进等复杂功能。
 * 专门用于公式内部的文本和公式混排。
 * 
 * 主要功能：
 * 1. 布局文本和内嵌公式
 * 2. 监听数据变化并自动更新布局
 * 3. 支持光标定位和点击交互
 * 4. 通知父元素布局变化
 */

#include "graphics/formula/GenericMathItem.h"
#include "core/document/math/GenericMathSpan.h"
#include "graphics/factory/MathItemFactory.h"
#include <QPainter>

namespace QtWordEditor {

/**
 * @brief 构造函数
 * @param span 对应的 GenericMathSpan 数据对象
 * @param parent 父 MathItem（可选）
 * 
 * 构造函数会执行以下操作：
 * 1. 配置布局引擎为 NoWrap 模式（不自动换行）
 * 2. 连接 GenericMathSpan 的 spansChanged 和 contentChanged 信号
 * 3. 初始化 MathItem 子项
 */
GenericMathItem::GenericMathItem(GenericMathSpan *span, MathItem *parent)
    : MathItem(span, parent)
    , m_layoutEngine(new TextBlockLayoutEngine())
    , m_mathItems()
    , m_boundingRect()
    , m_isUpdatingLayout(false)
{
    // 配置布局引擎为 NoWrap 模式，不自动换行
    m_layoutEngine->setWrapMode(TextBlockLayoutEngine::WrapMode::NoWrap);
    m_layoutEngine->setAvailableWidth(100000.0); // 足够大的宽度，不换行
    
    // 连接信号：当 GenericMathSpan 内容变化时更新布局
    if (span) {
        connect(span, &GenericMathSpan::spansChanged, this, &GenericMathItem::updateLayout);
        connect(span, &GenericMathSpan::contentChanged, this, &GenericMathItem::updateLayout);
    }
    
    // 初始化 MathItem 子项
    updateMathItems();
}

/**
 * @brief 析构函数
 * 
 * 释放布局引擎和 MathItem 子项。
 */
GenericMathItem::~GenericMathItem()
{
    delete m_layoutEngine;
    clearMathItems();
}

/**
 * @brief 更新 MathItem 子项
 * 
 * 该方法执行以下步骤：
 * 1. 清除旧的 MathItem
 * 2. 创建所有 MathItem 并获取它们的真实尺寸
 * 3. 使用真实尺寸执行布局
 * 4. 设置 MathItem 的位置并显示它们
 * 
 * 这样可以确保在布局前就能获取到公式的精确尺寸。
 */
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
        // 没有 MathItem，直接布局
        performLayout();
    } else {
        // 有 MathItem，使用真实尺寸布局
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

/**
 * @brief 清除 MathItem 子项
 */
void GenericMathItem::clearMathItems()
{
    qDeleteAll(m_mathItems);
    m_mathItems.clear();
}

/**
 * @brief 获取 InlineSpan 列表
 * @return InlineSpan 列表
 */
QList<InlineSpan*> GenericMathItem::getSpans() const
{
    GenericMathSpan *span = genericSpan();
    if (!span) {
        return QList<InlineSpan*>();
    }
    return span->spans();
}

/**
 * @brief 执行布局（无 MathItem 尺寸）
 * 
 * 使用 TextBlockLayoutEngine 进行布局，没有 MathItem 尺寸信息。
 */
void GenericMathItem::performLayout()
{
    QList<InlineSpan*> spans = getSpans();
    
    // 使用布局引擎进行布局
    m_layoutEngine->layout(spans);
    
    // 更新边界矩形
    m_boundingRect = QRectF(0, 0, m_layoutEngine->totalWidth(), m_layoutEngine->totalHeight());
}

/**
 * @brief 执行布局（使用给定的 MathSpan 尺寸）
 * @param mathSizeMap MathSpan 到尺寸的映射
 * @param mathBaselineMap MathSpan 到基线的映射
 * 
 * 使用 TextBlockLayoutEngine 进行布局，并使用提供的 MathItem 尺寸信息。
 */
void GenericMathItem::performLayoutWithMathSizes(const QHash<InlineSpan*, QSizeF> &mathSizeMap, 
                                                   const QHash<InlineSpan*, qreal> &mathBaselineMap)
{
    QList<InlineSpan*> spans = getSpans();
    
    // 使用布局引擎进行布局（提供 MathItem 尺寸）
    m_layoutEngine->layout(spans, mathSizeMap, mathBaselineMap);
    
    // 更新边界矩形
    m_boundingRect = QRectF(0, 0, m_layoutEngine->totalWidth(), m_layoutEngine->totalHeight());
}

/**
 * @brief 更新布局
 * 
 * 该方法执行以下操作：
 * 1. 防止无限递归调用（通过 m_isUpdatingLayout 标志）
 * 2. 更新所有 MathItem 子项的布局
 * 3. 重新创建 MathItem 并执行布局
 * 4. 设置基线（使用第一行的基线）
 * 5. 通知父元素布局已变化
 */
void GenericMathItem::updateLayout()
{
    // 防止无限递归调用
    if (m_isUpdatingLayout) {
        return;
    }
    
    m_isUpdatingLayout = true;
    
    // 更新所有 MathItem 子项的布局
    for (MathItem *mathItem : m_mathItems) {
        mathItem->updateLayout();
    }
    
    // 重新创建 MathItem 并执行布局
    updateMathItems();
    
    // 设置基线（使用第一行的基线）
    if (!m_layoutEngine->lines().isEmpty()) {
        m_baseline = m_layoutEngine->lines().first().maxBaseline;
    } else {
        m_baseline = 0;
    }
    
    // 通知父元素布局已变化
    notifyParentLayoutChanged();
    
    m_isUpdatingLayout = false;
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

int GenericMathItem::hitTestRegion(const QPointF &localPos) const
{
    qDebug() << "[GenericMathItem::hitTestRegion] localPos=" << localPos;
    
    // 遍历所有子 MathItem，检查点击位置是否在某个子 MathItem 上
    for (int i = 0; i < m_mathItems.size(); ++i) {
        MathItem *mathItem = m_mathItems.at(i);
        QRectF mathItemRect = mathItem->boundingRect();
        mathItemRect.translate(mathItem->pos());
        if (mathItemRect.contains(localPos)) {
            qDebug() << "  在子 MathItem 区域，索引=" << i;
            return i;
        }
    }
    
    // 如果不在任何子 MathItem 区域，返回 -1
    qDebug() << "  不在任何子 MathItem 区域";
    return -1;
}

} // namespace QtWordEditor
