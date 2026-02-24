/**
 * @file TextSpan.h
 * @brief 文本内联元素数据类
 * 
 * 继承自 InlineSpan，用于存储文本内容和样式。
 * 是文档中最基本的文本存储单元，支持丰富的文本操作和样式设置。
 */

#ifndef TEXTSPAN_H
#define TEXTSPAN_H

#include "InlineSpan.h"
#include "CharacterStyle.h"
#include <QString>
#include <QFont>
#include "core/Global.h"

namespace QtWordEditor {

class StyleManager;

/**
 * @brief 文本内联元素数据类
 * 
 * 继承自 InlineSpan，用于存储文本内容和样式。
 * 是文档中最基本的文本存储单元，支持丰富的文本操作和样式设置。
 * 支持命名样式和直接样式的组合使用。
 */
class TextSpan : public InlineSpan
{
public:
    /**
     * @brief 构造函数
     * @param parent 父对象指针，默认为nullptr
     */
    explicit TextSpan(QObject *parent = nullptr);
    
    /**
     * @brief 构造函数（带文本）
     * @param text 文本内容
     * @param parent 父对象指针，默认为nullptr
     */
    explicit TextSpan(const QString &text, QObject *parent = nullptr);
    
    /**
     * @brief 构造函数（带文本和样式）
     * @param text 文本内容
     * @param style 字符样式
     * @param parent 父对象指针，默认为nullptr
     */
    TextSpan(const QString &text, const CharacterStyle &style, QObject *parent = nullptr);
    
    /**
     * @brief 析构函数
     */
    ~TextSpan() override;

    // ========== InlineSpan 接口 ==========
    
    /**
     * @brief 获取内联元素类型
     * @return 固定返回 InlineSpan::Text
     */
    Type type() const override { return Text; }
    
    /**
     * @brief 获取文本长度
     * @return 文本字符数
     */
    int length() const override;
    
    /**
     * @brief 克隆文本内联元素
     * @return 克隆的 TextSpan 指针
     */
    InlineSpan *clone() const override;

    // ========== 文本内容 ==========
    
    /**
     * @brief 获取文本内容
     * @return 文本字符串
     */
    QString text() const;
    
    /**
     * @brief 设置文本内容
     * @param text 新的文本内容
     */
    void setText(const QString &text);
    
    /**
     * @brief 追加文本
     * @param text 要追加的文本
     */
    void append(const QString &text);
    
    /**
     * @brief 在指定位置插入文本
     * @param position 插入位置
     * @param text 要插入的文本
     */
    void insert(int position, const QString &text);
    
    /**
     * @brief 移除指定位置和长度的文本
     * @param position 起始位置
     * @param length 要移除的长度
     */
    void remove(int position, int length);

    // ========== 样式 - 传统接口（向后兼容） ==========
    
    /**
     * @brief 获取字符样式
     * @return 字符样式对象
     */
    CharacterStyle style() const;
    
    /**
     * @brief 设置字符样式
     * @param style 新的字符样式
     */
    void setStyle(const CharacterStyle &style);

    // ========== 命名样式 ==========
    
    /**
     * @brief 获取样式名称
     * @return 样式名称字符串
     */
    QString styleName() const;
    
    /**
     * @brief 设置样式名称
     * @param styleName 新的样式名称
     */
    void setStyleName(const QString &styleName);

    // ========== 直接样式（覆盖命名样式） ==========
    
    /**
     * @brief 获取直接样式
     * @return 直接样式对象
     */
    CharacterStyle directStyle() const;
    
    /**
     * @brief 设置直接样式
     * @param style 新的直接样式
     */
    void setDirectStyle(const CharacterStyle &style);

    // ========== 获取最终生效的样式（命名样式 + 直接样式） ==========
    
    /**
     * @brief 获取最终生效的样式
     * @param styleManager 样式管理器指针
     * @return 最终生效的字符样式
     */
    CharacterStyle effectiveStyle(const StyleManager *styleManager) const;

    // ========== 字体 ==========
    
    /**
     * @brief 获取字体
     * @return 字体对象
     */
    QFont font() const;

    // ========== 比较操作符 ==========
    
    /**
     * @brief 相等性比较
     * @param other 要比较的另一个 TextSpan
     * @return 如果相等返回true
     */
    bool operator==(const TextSpan &other) const;
    
    /**
     * @brief 不等性比较
     * @param other 要比较的另一个 TextSpan
     * @return 如果不等返回true
     */
    bool operator!=(const TextSpan &other) const;

private:
    QString m_text;               ///< 文本内容
    QString m_styleName;          ///< 命名样式名称
    CharacterStyle m_directStyle; ///< 直接样式（覆盖命名样式）
};

} // namespace QtWordEditor

#endif // TEXTSPAN_H
