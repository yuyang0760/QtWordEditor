#ifndef TEXTBLOCKITEM_H
#define TEXTBLOCKITEM_H

#include "BaseBlockItem.h"
#include <QGraphicsTextItem>
#include <QFont>
#include <QList>
#include <QHash>
#include "core/Global.h"
#include "graphics/items/TextBlockLayoutEngine.h"

namespace QtWordEditor {

class ParagraphBlock;
class InlineSpan;
class TextSpan;
class MathSpan;
class TextFragment;
class MathFormulaItem;

/**
 * @brief 文本块图形项类，将段落块渲染为格式化文本
 *
 * 该类负责在图形场景中显示段落块的内容，支持：
 * 1. 富文本格式显示
 * 2. 文本宽度和字体设置
 * 3. 几何形状的动态更新
 */
class TextBlockItem : public BaseBlockItem
{
public:
    /**
     * @brief 构造函数
     * @param block 关联的段落块对象
     * @param parent 父图形项指针，默认为nullptr
     */
    explicit TextBlockItem(ParagraphBlock *block, QGraphicsItem *parent = nullptr);
    
    /**
     * @brief 析构函数
     */
    ~TextBlockItem() override;

    /**
     * @brief 更新块内容显示
     * 从关联的段落块同步内容到图形项
     */
    void updateBlock() override;
    
    /**
     * @brief 设置文本显示宽度
     * @param width 新的文本宽度
     */
    void setTextWidth(qreal width);
    
    /**
     * @brief 获取当前文本宽度
     * @return 文本宽度值
     */
    qreal textWidth() const;
    
    /**
     * @brief 获取边界矩形
     * @return 图形项的边界矩形
     */
    QRectF boundingRect() const override;
    
    /**
     * @brief 绘制图形项
     * @param painter 绘制器
     * @param option 样式选项
     * @param widget 窗口部件
     */
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    
    /**
     * @brief 更新几何形状
     * 重新计算和设置图形项的几何属性
     */
    void updateGeometry();

    // ========== 视图-数据映射 ==========
    /**
     * @brief 根据TextSpan获取对应的TextFragment
     * @param span TextSpan指针
     * @return TextFragment指针，找不到返回nullptr
     */
    TextFragment* getTextFragmentForSpan(InlineSpan* span) const;
    
    /**
     * @brief 根据MathSpan获取对应的MathFormulaItem
     * @param span MathSpan指针
     * @return MathFormulaItem指针，找不到返回nullptr
     */
    MathFormulaItem* getMathItemForSpan(MathSpan* span) const;

private:
    /** @brief 从块数据创建内容项（TextFragment和MathFormulaItem） */
    void createContentItemsFromBlock();
    
    /** @brief 使用TextBlockLayoutEngine执行布局 */
    void performLayout();
    
    /** @brief 清除内容项 */
    void clearContentItems();
    
    qreal m_textWidth;              ///< 文本显示宽度
    TextBlockLayoutEngine m_layoutEngine;  ///< 布局引擎
    QList<QGraphicsItem*> m_contentItems;  ///< 内容项列表（TextFragment或MathFormulaItem）
    QHash<InlineSpan*, TextFragment*> m_textFragmentMap;  ///< TextSpan到TextFragment的映射
    QHash<MathSpan*, MathFormulaItem*> m_mathItemMap;     ///< MathSpan到MathFormulaItem的映射
};

} // namespace QtWordEditor

#endif // TEXTBLOCKITEM_H
