#ifndef CHARACTERSTYLE_H
#define CHARACTERSTYLE_H

#include <QFont>
#include <QColor>
#include <QSharedData>
#include <QSharedDataPointer>
#include "core/Global.h"

namespace QtWordEditor {

class CharacterStyleData;

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

private:
    QSharedDataPointer<CharacterStyleData> d;  ///< 隐式共享数据指针
};

} // namespace QtWordEditor

#endif // CHARACTERSTYLE_H