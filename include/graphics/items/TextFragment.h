#ifndef TEXTFRAGMENT_H
#define TEXTFRAGMENT_H

#include <QGraphicsItem>
#include <QString>
#include <QFont>
#include <QColor>
#include "core/document/CharacterStyle.h"
#include "core/Global.h"

namespace QtWordEditor {

/**
 * @brief 文本片段类
 *
 * 自己渲染文本，不使用 QGraphicsTextItem。
 * 负责：
 * 1. 文本测量（宽度、高度、基线）
 * 2. 文本渲染（使用 QPainter）
 * 3. 字符样式应用
 */
class TextFragment : public QGraphicsItem
{
public:
    // 类型 ID
    enum { Type = UserType + 1010 };

    /**
     * @brief 构造函数
     * @param text 文本内容
     * @param style 字符样式
     * @param parent 父图形项指针
     */
    explicit TextFragment(const QString &text, const CharacterStyle &style, 
                          QGraphicsItem *parent = nullptr);
    
    /**
     * @brief 析构函数
     */
    ~TextFragment() override;

    // ========== 文本内容访问 ==========
    
    /**
     * @brief 获取文本内容
     * @return 文本
     */
    QString text() const;
    
    /**
     * @brief 设置文本内容
     * @param text 文本
     */
    void setText(const QString &text);
    
    /**
     * @brief 获取字符样式
     * @return 字符样式
     */
    CharacterStyle style() const;
    
    /**
     * @brief 设置字符样式
     * @param style 字符样式
     */
    void setStyle(const CharacterStyle &style);

    // ========== 尺寸和基线 ==========
    
    /**
     * @brief 获取文本宽度
     * @return 宽度
     */
    qreal width() const;
    
    /**
     * @brief 获取文本高度
     * @return 高度
     */
    qreal height() const;
    
    /**
     * @brief 获取基线位置（从顶部到基线的距离）
     * @return 基线位置
     */
    qreal baseline() const;

    // ========== QGraphicsItem 接口 ==========
    
    int type() const override;
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    /**
     * @brief 计算文本尺寸（内部方法）
     */
    void calculateSize();
    
    /**
     * @brief 从 CharacterStyle 创建 QFont
     * @param style 字符样式
     * @return QFont
     */
    QFont createFontFromStyle(const CharacterStyle &style) const;

private:
    QString m_text;           ///< 文本内容
    CharacterStyle m_style;   ///< 字符样式
    QFont m_font;             ///< 字体
    QColor m_textColor;       ///< 文本颜色
    QRectF m_boundingRect;    ///< 边界矩形
    qreal m_baseline;         ///< 基线位置
};

} // namespace QtWordEditor

#endif // TEXTFRAGMENT_H
