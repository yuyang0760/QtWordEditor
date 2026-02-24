/**
 * @file MathItem.cpp
 * @brief 所有公式视图元素的基类实现
 * 
 * 该类是所有数学公式视图元素的基类，提供了通用的接口和功能。
 * 包括：
 * - 布局更新
 * - 子项管理
 * - 鼠标和键盘事件处理
 * - 光标定位
 * - 通知父元素布局变化的机制
 */

#include "graphics/formula/MathItem.h"
#include "core/document/MathSpan.h"
#include "graphics/items/TextBlockItem.h"

namespace QtWordEditor {

/**
 * @brief 构造函数
 * @param span 关联的 MathSpan 数据对象
 * @param parent 父 MathItem（可选）
 */
MathItem::MathItem(MathSpan *span, MathItem *parent)
    : QGraphicsItem(parent)
    , m_boundingRect()
    , m_baseline(0)
    , m_span(span)
{
    // 设置 QGraphicsItem 标志
    setFlag(QGraphicsItem::ItemIsSelectable, false);
    setFlag(QGraphicsItem::ItemIsFocusable, false);
    
    // 接受左键事件，允许点击定位光标
    setAcceptedMouseButtons(Qt::LeftButton);
}

/**
 * @brief 更新布局
 * 
 * 默认实现为空，子类需要重写此方法来实现自己的布局逻辑。
 */
void MathItem::updateLayout()
{
    // 默认实现：子类重写
}

/**
 * @brief 获取基线位置
 * @return 基线的 y 坐标
 */
qreal MathItem::baseline() const
{
    return m_baseline;
}

/**
 * @brief 在指定位置插入子 MathItem
 * @param index 插入位置
 * @param item 要插入的 MathItem
 * 
 * 默认实现：只发出警告，子类（容器类型）需要重写此方法。
 */
void MathItem::insertChild(int index, MathItem *item)
{
    Q_UNUSED(index);
    Q_UNUSED(item);
    qWarning() << "MathItem::insertChild: Not a container type";
}

/**
 * @brief 在末尾追加子 MathItem
 * @param item 要追加的 MathItem
 * 
 * 默认实现：只发出警告，子类（容器类型）需要重写此方法。
 */
void MathItem::appendChild(MathItem *item)
{
    Q_UNUSED(item);
    qWarning() << "MathItem::appendChild: Not a container type";
}

/**
 * @brief 移除指定的子 MathItem
 * @param item 要移除的 MathItem
 * 
 * 默认实现：只发出警告，子类（容器类型）需要重写此方法。
 */
void MathItem::removeChild(MathItem *item)
{
    Q_UNUSED(item);
    qWarning() << "MathItem::removeChild: Not a container type";
}

/**
 * @brief 获取所有子 MathItem 列表
 * @return 子 MathItem 列表
 * 
 * 默认实现：返回空列表，子类（容器类型）需要重写此方法。
 */
QList<MathItem*> MathItem::children() const
{
    return QList<MathItem*>();
}

/**
 * @brief 获取子 MathItem 的数量
 * @return 子 MathItem 的数量
 * 
 * 默认实现：返回 0，子类（容器类型）需要重写此方法。
 */
int MathItem::childCount() const
{
    return 0;
}

/**
 * @brief 获取指定位置的子 MathItem
 * @param index 位置索引
 * @return 子 MathItem 指针
 * 
 * 默认实现：返回 nullptr，子类（容器类型）需要重写此方法。
 */
MathItem *MathItem::childAt(int index) const
{
    Q_UNUSED(index);
    return nullptr;
}

/**
 * @brief 获取子 MathItem 在父元素中的索引
 * @param child 子 MathItem 指针
 * @return 索引位置
 * 
 * 默认实现：返回 -1，子类（容器类型）需要重写此方法。
 */
int MathItem::indexOfChild(MathItem *child) const
{
    Q_UNUSED(child);
    return -1;
}

/**
 * @brief 获取关联的 MathSpan 数据对象
 * @return MathSpan 指针
 */
MathSpan *MathItem::mathSpan() const
{
    return m_span;
}

/**
 * @brief 获取父 MathItem
 * @return 父 MathItem 指针，如果没有父 MathItem 则返回 nullptr
 */
MathItem *MathItem::parentMathItem() const
{
    return dynamic_cast<MathItem*>(parentItem());
}

/**
 * @brief 通知父元素布局已变化
 * 
 * 这个方法实现了一个重要的通知机制：
 * 1. 首先检查是否有父 MathItem，如果有，则调用父项的 updateLayout()
 * 2. 如果没有父 MathItem（说明是根 MathItem），则向上遍历 QGraphicsItem 层次结构
 * 3. 查找类型为 TextBlockItem（Type = UserType + 1001）的项
 * 4. 找到后调用 TextBlockItem::safeUpdateLayout() 来重新布局整个段落
 * 
 * 这样可以确保：
 * - 当公式尺寸变化时，父级公式也会更新
 * - 当根公式尺寸变化时，整个文本段落也会重新布局
 * - 后面的文本会自动向后移动，不会被公式覆盖
 */
void MathItem::notifyParentLayoutChanged()
{
    // 首先尝试获取父 MathItem
    MathItem *parent = parentMathItem();
    
    if (parent) {
        // 有父 MathItem，调用父项的 updateLayout()
        parent->updateLayout();
    } else {
        // 没有父 MathItem，说明这是根 MathItem，需要通知 TextBlockItem 更新布局
        QGraphicsItem *p = parentItem();
        
        // 向上遍历 QGraphicsItem 层次结构
        while (p) {
            // 检查是否是 TextBlockItem（Type = UserType + 1001）
            if (p->type() == QGraphicsItem::UserType + 1001) {
                // 找到了 TextBlockItem，调用 safeUpdateLayout() 重新布局整个段落
                TextBlockItem *textBlockItem = static_cast<TextBlockItem*>(p);
                textBlockItem->safeUpdateLayout();
                return;
            }
            p = p->parentItem();
        }
    }
}

/**
 * @brief 鼠标按下事件处理
 * @param event 鼠标事件
 * 
 * 调用基类处理后，设置焦点。
 */
void MathItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mousePressEvent(event);
    setFocus();
}

/**
 * @brief 鼠标移动事件处理
 * @param event 鼠标事件
 */
void MathItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mouseMoveEvent(event);
}

/**
 * @brief 鼠标释放事件处理
 * @param event 鼠标事件
 */
void MathItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mouseReleaseEvent(event);
}

/**
 * @brief 键盘按下事件处理
 * @param event 键盘事件
 */
void MathItem::keyPressEvent(QKeyEvent *event)
{
    QGraphicsItem::keyPressEvent(event);
}

/**
 * @brief 判断一个点在公式的哪个子 MathItem 区域
 * @param localPos 相对于自身的局部坐标
 * @return 子 MathItem 的索引，如果不在任何子 MathItem 区域返回 -1
 * 
 * 默认实现：返回 -1，子类（容器类型）需要重写此方法。
 */
int MathItem::hitTestRegion(const QPointF &localPos) const
{
    Q_UNUSED(localPos);
    // 默认实现：非容器类型返回 -1
    return -1;
}

} // namespace QtWordEditor
