#ifndef TEXTBLOCKITEM_H
#define TEXTBLOCKITEM_H

#include "BaseBlockItem.h"
#include <QGraphicsTextItem>
#include <QFont>
#include <QList>
#include <QHash>
#include <QSizeF>
#include "core/Global.h"
#include "core/document/InlineSpan.h"
#include "graphics/items/TextBlockLayoutEngine.h"

namespace QtWordEditor {

class ParagraphBlock;
class MathItem;
class MathSpan;
class MathCursor;
class UnifiedCursorVisual;
class RowContainerItem;

/**
 * @brief 文本块图形项类（直接绘制版）
 *
 * 不再使用 TextFragment！直接从 ParagraphBlock 读取 InlineSpan 列表，直接绘制！
 * 保留原有接口以确保兼容性。
 */
class TextBlockItem : public BaseBlockItem
{
public:
    // 类型 ID
    enum { Type = UserType + 1001 };

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
     * @brief 获取内部的文本图形项
     * @return 指向QGraphicsTextItem的指针（为了兼容性保留，返回nullptr）
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
     * @brief 绘制图形项
     * @param painter 画家指针
     * @param option 样式选项
     * @param widget 窗口指针
     */
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    
    /**
     * @brief 更新几何形状
     * 重新计算和设置图形项的几何属性
     */
    void updateGeometry();
    
    /**
     * @brief 获取类型
     * @return 类型ID
     */
    int type() const override { return Type; }

    // ========== 光标位置计算 ==========
    
    /**
     * @brief 根据相对于 TextBlockItem 的局部坐标获取全局字符偏移
     * @param localPos 局部坐标
     * @return 全局字符偏移（从段落开头算起）
     */
    int hitTest(const QPointF &localPos) const;
    
    /**
     * @brief 光标视觉位置信息
     */
    struct CursorVisualInfo {
        QPointF position;  ///< 视觉位置（相对于 TextBlockItem）
        qreal height;      ///< 光标高度
    };
    
    /**
     * @brief 根据全局字符偏移获取视觉位置
     * @param globalOffset 全局字符偏移
     * @return 视觉位置信息
     */
    CursorVisualInfo cursorPositionAt(int globalOffset) const;

    /**
     * @brief 计算选择矩形（相对于 TextBlockItem 局部坐标）
     * @param startOffset 选择起始偏移
     * @param endOffset 选择结束偏移
     * @return 选择矩形列表
     */
    QList<QRectF> selectionRects(int startOffset, int endOffset) const;

private:
    // ========== 内部方法 ==========
    
    /**
     * @brief 获取段落的 InlineSpan 列表
     */
    QList<InlineSpan*> getSpans() const;
    
    /**
     * @brief 执行布局
     */
    void performLayout();
    
    /**
     * @brief 执行布局（使用给定的MathSpan尺寸）
     */
    void performLayoutWithMathSizes(const QHash<InlineSpan*, QSizeF> &mathSizeMap, 
                                     const QHash<InlineSpan*, qreal> &mathBaselineMap);
    
    /**
     * @brief 应用段落缩进（左缩进、右缩进）
     */
    void applyParagraphIndent();

public:
    // ========== 选择范围 ==========
    
    /**
     * @brief 设置选择范围
     * @param startOffset 选择起始偏移
     * @param endOffset 选择结束偏移
     */
    void setSelectionRange(int startOffset, int endOffset);
    
    /**
     * @brief 清除选择范围
     */
    void clearSelection();

    // ========== MathItem 管理 ==========
    
    /**
     * @brief 清除所有 MathItem
     */
    void clearMathItems();
    
    // ========== 公式编辑模式 ==========
    
    /**
     * @brief 判断是否处于公式编辑模式
     * @return true 表示处于公式编辑模式
     */
    bool isInMathEditMode() const;
    
    /**
     * @brief 进入公式编辑模式
     * @param mathSpan 要编辑的 MathSpan
     */
    void enterMathEditMode(MathSpan *mathSpan);
    
    /**
     * @brief 退出公式编辑模式
     */
    void exitMathEditMode();
    
    /**
     * @brief 获取当前的 MathCursor
     * @return MathCursor 指针
     */
    MathCursor *mathCursor() const;
    
    // ========== 统一光标（新）相关方法 ==========
    
    /**
     * @brief 设置是否使用统一光标（新）
     * @param useUnified true=使用统一光标，false=使用旧光标
     */
    void setUseUnifiedCursor(bool useUnified);
    
    /**
     * @brief 获取是否使用统一光标（新）
     * @return true=使用统一光标
     */
    bool useUnifiedCursor() const;
    
    // ========== 事件处理 ==========
    
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    TextBlockLayoutEngine *m_layoutEngine;  ///< 布局引擎
    qreal m_textWidth;                      ///< 文本显示宽度
    qreal m_leftIndent;                     ///< 左缩进
    qreal m_rightIndent;                    ///< 右缩进
    QRectF m_boundingRect;                  ///< 边界矩形
    int m_selectionStartOffset;             ///< 选择起始偏移
    int m_selectionEndOffset;               ///< 选择结束偏移
    QList<QGraphicsItem*> m_mathItems;     ///< MathItem 子项列表
    
    // ========== 公式编辑模式 ==========
    bool m_inMathEditMode;                  ///< 是否处于公式编辑模式
    MathItem *m_rootMathItem;               ///< 当前编辑的根 MathItem
    MathCursor *m_mathCursor;               ///< 公式光标
    
    // ========== 点击信息存储（临时） ==========
    MathItem *m_clickedMathItem;            ///< 被点击的 MathItem
    int m_clickedRegion;                     ///< 被点击的区域（0=分子，1=分母，-1=其他）
    QPointF m_clickedLocalPos;               ///< 被点击的局部坐标
    
    // ========== 统一光标（新） ==========
    bool m_useUnifiedCursor;                 ///< 是否使用统一光标（默认false）
};

} // namespace QtWordEditor

#endif // TEXTBLOCKITEM_H
