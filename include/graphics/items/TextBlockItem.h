#ifndef TEXTBLOCKITEM_H
#define TEXTBLOCKITEM_H

#include "BaseBlockItem.h"
#include <QGraphicsTextItem>
#include <QFont>
#include <QList>
#include "core/Global.h"
#include "core/document/Span.h"

namespace QtWordEditor {

class ParagraphBlock;

/**
 * @brief 文本块图形项类，将段落块渲染为格式化文本
 *
 * 该类负责在图形场景中显示段落块的内容，支持：
 * 1. 富文本格式显示
 * 2. 文本宽度和字体设置
 * 3. 几何形状的动态更新
 * 4. 与底层数据块的同步
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
     * @brief 更新块内容显示
     * @param forceUpdate 是否强制更新，即使检测到内容没有变化
     */
    void updateBlock(bool forceUpdate);
    
    /**
     * @brief 获取内部的文本图形项
     * @return 指向QGraphicsTextItem的指针
     */
    QGraphicsTextItem *textItem() const;
    
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
     * @brief 设置字体
     * @param font 新的字体设置
     */
    void setFont(const QFont &font);
    
    /**
     * @brief 获取当前字体
     * @return 当前字体设置
     */
    QFont font() const;
    
    /**
     * @brief 设置纯文本内容
     * @param text 新的文本内容
     */
    void setPlainText(const QString &text);
    
    /**
     * @brief 获取纯文本内容
     * @return 当前的纯文本内容
     */
    QString toPlainText() const;
    
    /**
     * @brief 获取边界矩形
     * @return 图形项的边界矩形
     */
    QRectF boundingRect() const override;
    
    /**
     * @brief 更新几何形状
     * 重新计算和设置图形项的几何属性
     */
    void updateGeometry();

private:
    /** @brief 初始化内部文本图形项 */
    void initializeTextItem();
    
    /** @brief 更新边界矩形 */
    void updateBoundingRect();
    
    /** @brief 从块数据应用富文本格式 */
    void applyRichTextFromBlock();
    
    QGraphicsTextItem *m_textItem;  ///< 内部文本图形项
    qreal m_textWidth;              ///< 文本显示宽度
    
    // 缓存上次渲染的内容，避免不必要的重建
    QList<Span> m_cachedSpans;      ///< 上次渲染的 Span 列表
};

} // namespace QtWordEditor

#endif // TEXTBLOCKITEM_H
