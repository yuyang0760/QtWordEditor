#ifndef CHARACTERSTYLE_H
#define CHARACTERSTYLE_H

#include <QFont>
#include <QColor>
#include <QSharedData>
#include <QSharedDataPointer>
#include <QFlags>
#include "core/Global.h"

namespace QtWordEditor {

class CharacterStyleData;

/**
 * @brief 字符样式属性枚举
 * 用于标记哪些属性已被显式设置
 */
enum class CharacterStyleProperty {
    FontFamily     = 1 << 0,  ///< 字体族
    FontSize       = 1 << 1,  ///< 字体大小
    Bold           = 1 << 2,  ///< 粗体
    Italic         = 1 << 3,  ///< 斜体
    Underline      = 1 << 4,  ///< 下划线
    StrikeOut      = 1 << 5,  ///< 删除线
    TextColor      = 1 << 6,  ///< 文字颜色
    BackgroundColor = 1 << 7, ///< 背景颜色
    LetterSpacing  = 1 << 8   ///< 字符间距
};

Q_DECLARE_FLAGS(CharacterStylePropertyFlags, CharacterStyleProperty)
Q_DECLARE_OPERATORS_FOR_FLAGS(CharacterStylePropertyFlags)

/**
 * @brief 字符样式类，封装字符级别的格式化属性
 *
 * 该类使用隐式共享（写时复制）技术来提高复制效率，
 * 包含字体、颜色、间距等字符级格式设置。
 */
class CharacterStyle
{
public:
    /** @brief 默认构造函数 */
    CharacterStyle();
    
    /** @brief 拷贝构造函数 */
    CharacterStyle(const CharacterStyle &other);
    
    /** @brief 赋值操作符 */
    CharacterStyle &operator=(const CharacterStyle &other);
    
    /** @brief 析构函数 */
    ~CharacterStyle();

    // ========== 字体属性相关方法 ==========
    
    /**
     * @brief 获取完整字体设置
     * @return QFont对象包含所有字体属性
     */
    QFont font() const;
    
    /**
     * @brief 设置完整字体
     * @param font 新的字体设置
     */
    void setFont(const QFont &font);

    /**
     * @brief 获取字体族名称
     * @return 字体族字符串
     */
    QString fontFamily() const;
    
    /**
     * @brief 设置字体族
     * @param family 字体族名称
     */
    void setFontFamily(const QString &family);

    /**
     * @brief 获取字体大小
     * @return 字体大小（磅值）
     */
    int fontSize() const;
    
    /**
     * @brief 设置字体大小
     * @param size 字体大小（磅值）
     */
    void setFontSize(int size);

    /**
     * @brief 获取粗体状态
     * @return 如果是粗体返回true，否则返回false
     */
    bool bold() const;
    
    /**
     * @brief 设置粗体状态
     * @param bold 是否启用粗体
     */
    void setBold(bool bold);

    /**
     * @brief 获取斜体状态
     * @return 如果是斜体返回true，否则返回false
     */
    bool italic() const;
    
    /**
     * @brief 设置斜体状态
     * @param italic 是否启用斜体
     */
    void setItalic(bool italic);

    /**
     * @brief 获取下划线状态
     * @return 如果有下划线返回true，否则返回false
     */
    bool underline() const;
    
    /**
     * @brief 设置下划线状态
     * @param underline 是否启用下划线
     */
    void setUnderline(bool underline);

    /**
     * @brief 获取删除线状态
     * @return 如果有删除线返回true，否则返回false
     */
    bool strikeOut() const;
    
    /**
     * @brief 设置删除线状态
     * @param strikeOut 是否启用删除线
     */
    void setStrikeOut(bool strikeOut);

    // ========== 颜色相关方法 ==========
    
    /**
     * @brief 获取文字颜色
     * @return 文字颜色值
     */
    QColor textColor() const;
    
    /**
     * @brief 设置文字颜色
     * @param color 新的文字颜色
     */
    void setTextColor(const QColor &color);

    /**
     * @brief 获取背景颜色
     * @return 背景颜色值
     */
    QColor backgroundColor() const;
    
    /**
     * @brief 设置背景颜色
     * @param color 新的背景颜色
     */
    void setBackgroundColor(const QColor &color);

    // ========== 字符间距相关方法 ==========
    
    /**
     * @brief 获取字符间距
     * @return 字符间距值
     */
    qreal letterSpacing() const;
    
    /**
     * @brief 设置字符间距
     * @param spacing 新的字符间距
     */
    void setLetterSpacing(qreal spacing);

    // ========== 比较操作符 ==========
    
    /**
     * @brief 相等性比较
     * @param other 要比较的另一个字符样式
     * @return 如果两个样式相等返回true
     */
    bool operator==(const CharacterStyle &other) const;
    
    /**
     * @brief 不等性比较
     * @param other 要比较的另一个字符样式
     * @return 如果两个样式不等返回true
     */
    bool operator!=(const CharacterStyle &other) const;

    // ========== 重置方法 ==========
    
    /**
     * @brief 重置为默认样式
     * 将所有属性恢复为默认值
     */
    void reset();

    // ========== 属性设置标记相关方法 ==========
    
    /**
     * @brief 检查某个属性是否已被显式设置
     * @param property 要检查的属性
     * @return 如果属性已设置返回true，否则返回false
     */
    bool isPropertySet(CharacterStyleProperty property) const;
    
    /**
     * @brief 清除某个属性的设置标记，将其恢复为默认值
     * @param property 要清除的属性
     */
    void clearProperty(CharacterStyleProperty property);
    
    /**
     * @brief 清除所有属性的设置标记，将所有属性恢复为默认值
     */
    void clearAllProperties();
    
    /**
     * @brief 合并两个样式
     * 只合并另一个样式中已显式设置的属性
     * @param other 要合并的样式
     * @return 合并后的新样式
     */
    CharacterStyle mergeWith(const CharacterStyle &other) const;

private:
    QSharedDataPointer<CharacterStyleData> d;  ///< 隐式共享数据指针
};

} // namespace QtWordEditor

#endif // CHARACTERSTYLE_H