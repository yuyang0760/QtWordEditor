#ifndef TEXTBLOCKLAYOUTENGINE_H
#define TEXTBLOCKLAYOUTENGINE_H

#include <QList>
#include <QPointF>
#include <QRectF>
#include <QString>
#include <QFont>
#include <QTextLine>
#include <QFontMetricsF>
#include "core/document/ParagraphStyle.h"
#include "core/document/CharacterStyle.h"
#include "core/document/Span.h"

namespace QtWordEditor {

/**
 * @brief 文本块布局引擎类
 *
 * 负责文本的测量、换行、对齐和基线对齐等排版工作
 * 直接基于 Span 工作，不需要 QGraphicsItem！
 */
class TextBlockLayoutEngine
{
public:
    /**
     * @brief 换行模式
     */
    enum class WrapMode {
        NoWrap,         ///< 不换行
        WrapAtWordBoundary,  ///< 在单词边界换行
        WrapAnywhere    ///< 任意位置换行
    };

    /**
     * @brief 单个内容项的信息（对应一个 Span 的某一行片段）
     */
    struct LayoutItem {
        int spanIndex;              ///< 对应的 Span 索引
        QString text;               ///< 文本内容（可能是 Span 的某一行）
        QString fullSpanText;       ///< 整个 Span 的完整文本
        CharacterStyle style;       ///< 字符样式
        qreal width;                ///< 宽度
        qreal height;               ///< 高度
        qreal ascent;               ///< 上升高度（从基线到顶部）
        qreal descent;              ///< 下降高度（从基线到底部）
        QPointF position;           ///< 位置（相对于 TextBlockItem 的左上角）
        QFont font;                 ///< 字体（用于绘制）
        int startOffsetInSpan;      ///< 在原 Span 中的起始偏移
        int endOffsetInSpan;        ///< 在原 Span 中的结束偏移
        int globalStartOffset;      ///< 全局起始偏移（从段落开头算起）
        int globalEndOffset;        ///< 全局结束偏移
    };

    /**
     * @brief 行信息
     */
    struct LineInfo {
        QRectF rect;              ///< 行的矩形
        qreal maxBaseline;        ///< 该行的最大基线
        QList<int> itemIndices;   ///< 该行包含的 LayoutItem 的索引
    };

    /**
     * @brief 光标命中结果
     */
    struct CursorHitResult {
        int globalOffset;         ///< 全局字符偏移（从段落开头算起）
        int lineIndex;            ///< 所在行索引
        int itemIndex;            ///< 所在 LayoutItem 索引
        int offsetInItem;         ///< 在 LayoutItem 中的偏移
    };

    /**
     * @brief 光标视觉位置结果
     */
    struct CursorVisualResult {
        QPointF position;         ///< 视觉位置（相对于段落）
        qreal height;             ///< 光标高度
    };

    /**
     * @brief 构造函数
     */
    TextBlockLayoutEngine();
    
    /**
     * @brief 析构函数
     */
    ~TextBlockLayoutEngine();

    /**
     * @brief 设置可用宽度
     * @param width 可用宽度
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

    /**
     * @brief 执行布局
     * @param spans 段落的 Span 列表
     */
    void layout(const QList<Span> &spans);

    /**
     * @brief 获取布局后的所有项
     * @return 布局项列表
     */
    const QList<LayoutItem> &layoutItems() const;
    
    /**
     * @brief 获取布局后的所有行
     * @return 行信息列表
     */
    const QList<LineInfo> &lines() const;
    
    /**
     * @brief 获取总宽度
     * @return 总宽度
     */
    qreal totalWidth() const;
    
    /**
     * @brief 获取总高度
     * @return 总高度
     */
    qreal totalHeight() const;

    // ========== 光标位置计算 ==========
    
    /**
     * @brief 根据局部坐标找到光标位置
     * @param localPos 相对于段落的局部坐标
     * @param spans 段落的 Span 列表
     * @return 命中结果
     */
    CursorHitResult hitTest(const QPointF &localPos, const QList<Span> &spans) const;
    
    /**
     * @brief 根据全局偏移找到光标视觉位置
     * @param globalOffset 全局字符偏移（从段落开头算起）
     * @param spans 段落的 Span 列表
     * @return 视觉位置结果
     */
    CursorVisualResult cursorPositionAt(int globalOffset, const QList<Span> &spans) const;

private:
    /**
     * @brief 从 CharacterStyle 创建 QFont
     * @param style 字符样式
     * @return QFont
     */
    QFont createFontFromStyle(const CharacterStyle &style) const;
    
    /**
     * @brief 完成一行的布局
     */
    void finishLine(QList<int> &itemIndices, QList<LayoutItem> &allItems, 
                    qreal lineWidth, qreal lineY, qreal lineHeight, qreal lineMaxAscent);
    
    /**
     * @brief 从 LayoutItem 列表完成一行的布局
     */
    void finishLineFromItems(QList<LayoutItem> &items, qreal lineY);
    
    /**
     * @brief 清除所有布局结果
     */
    void clear();

private:
    qreal m_availableWidth;         ///< 可用宽度
    ParagraphStyle m_paragraphStyle;///< 段落样式
    WrapMode m_wrapMode;             ///< 换行模式
    
    QList<LayoutItem> m_layoutItems;///< 所有布局项（对应 Span）
    QList<LineInfo> m_lines;        ///< 所有行信息
    qreal m_totalWidth;              ///< 总宽度
    qreal m_totalHeight;             ///< 总高度
};

} // namespace QtWordEditor

#endif // TEXTBLOCKLAYOUTENGINE_H
