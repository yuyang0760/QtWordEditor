#ifndef TEXTBLOCKLAYOUTENGINE_H
#define TEXTBLOCKLAYOUTENGINE_H

#include <QList>
#include <QRectF>
#include <QPointF>
#include <QHash>
#include <QGraphicsItem>
#include "core/document/ParagraphStyle.h"
#include "core/Global.h"

namespace QtWordEditor {

class TextFragment;

/**
 * @brief 文本块布局引擎
 *
 * 这是完全重写 TextBlockItem 的核心！
 * 负责：
 * 1. 文本测量和换行
 * 2. 文本对齐
 * 3. 基线对齐（专业排版）
 * 4. 对象定位（嵌入公式）
 * 5. 计算整体尺寸
 */
class TextBlockLayoutEngine
{
public:
    /**
     * @brief 换行模式
     */
    enum class WrapMode {
        NoWrap,         ///< 不换行
        WordWrap,       ///< 单词换行
        WrapAnywhere    ///< 字符换行
    };

    /**
     * @brief 构造函数
     */
    TextBlockLayoutEngine();
    
    /**
     * @brief 析构函数
     */
    ~TextBlockLayoutEngine();

    // ========== 布局设置 ==========
    
    /**
     * @brief 设置可用宽度
     * @param width 宽度
     */
    void setAvailableWidth(qreal width);
    
    /**
     * @brief 设置段落样式
     * @param style 段落样式
     */
    void setParagraphStyle(const ParagraphStyle &style);
    
    /**
     * @brief 设置换行模式
     * @param mode 换行模式
     */
    void setWrapMode(WrapMode mode);

    // ========== 布局执行 ==========
    
    /**
     * @brief 执行布局
     * @param items 所有内容项（TextFragment + MathFormulaGraphicsItem）
     */
    void layout(const QList<QGraphicsItem*> &items);
    
    /**
     * @brief 清除布局结果
     */
    void clear();

    // ========== 布局结果获取 ==========
    
    /**
     * @brief 获取计算后的总高度
     * @return 高度
     */
    qreal totalHeight() const;
    
    /**
     * @brief 获取计算后的总宽度
     * @return 宽度
     */
    qreal totalWidth() const;
    
    /**
     * @brief 获取某个项的位置
     * @param item 项指针
     * @return 位置（相对于 TextBlockItem）
     */
    QPointF positionForItem(QGraphicsItem *item) const;
    
    /**
     * @brief 获取所有行的信息
     * @return 行列表
     */
    QList<QRectF> lines() const;

private:
    /**
     * @brief 一行的信息
     */
    struct LineInfo {
        QRectF rect;              ///< 行的矩形
        qreal maxBaseline;        ///< 该行的最大基线（用于对齐）
        QList<QGraphicsItem*> items;  ///< 该行的所有项
    };

    // ========== 内部布局方法 ==========
    
    /**
     * @brief 将项分配到行
     */
    void assignItemsToLines(const QList<QGraphicsItem*> &items);
    
    /**
     * @brief 计算每一行的基线
     */
    void calculateLineBaselines();
    
    /**
     * @brief 应用文本对齐
     */
    void applyAlignment();
    
    /**
     * @brief 定位所有项
     */
    void positionItems();
    
    /**
     * @brief 计算整体尺寸
     */
    void calculateTotalSize();
    
    /**
     * @brief 获取项的宽度
     */
    qreal itemWidth(QGraphicsItem *item) const;
    
    /**
     * @brief 获取项的高度
     */
    qreal itemHeight(QGraphicsItem *item) const;
    
    /**
     * @brief 获取项的基线
     */
    qreal itemBaseline(QGraphicsItem *item) const;

private:
    qreal m_availableWidth;          ///< 可用宽度
    ParagraphStyle m_paragraphStyle; ///< 段落样式
    WrapMode m_wrapMode;             ///< 换行模式
    
    QList<LineInfo> m_lines;         ///< 所有行的信息
    QHash<QGraphicsItem*, QPointF> m_itemPositions;  ///< 项的位置
    qreal m_totalWidth;              ///< 总宽度
    qreal m_totalHeight;             ///< 总高度
};

} // namespace QtWordEditor

#endif // TEXTBLOCKLAYOUTENGINE_H
