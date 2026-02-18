#ifndef PARAGRAPHSTYLE_H
#define PARAGRAPHSTYLE_H

#include <QSharedData>
#include <QSharedDataPointer>
#include "core/Global.h"

namespace QtWordEditor {

/**
 * @brief 段落对齐方式枚举
 * 定义段落文本的对齐选项
 */
enum class ParagraphAlignment {
    AlignLeft,      ///< 左对齐
    AlignCenter,    ///< 居中对齐
    AlignRight,     ///< 右对齐
    AlignJustify    ///< 两端对齐
};

class ParagraphStyleData;

/**
 * @brief 段落样式类，封装段落级别的格式化属性
 *
 * 该类管理段落的整体格式设置，包括对齐方式、缩进、行距和间距等属性。
 * 使用隐式共享技术提高性能效率。
 */
class ParagraphStyle
{
public:
    /** @brief 默认构造函数 */
    ParagraphStyle();
    
    /** @brief 拷贝构造函数 */
    ParagraphStyle(const ParagraphStyle &other);
    
    /** @brief 赋值操作符 */
    ParagraphStyle &operator=(const ParagraphStyle &other);
    
    /** @brief 析构函数 */
    ~ParagraphStyle();

    // ========== 对齐方式相关方法 ==========
    
    /**
     * @brief 获取段落对齐方式
     * @return 当前的对齐方式枚举值
     */
    ParagraphAlignment alignment() const;
    
    /**
     * @brief 设置段落对齐方式
     * @param align 新的对齐方式
     */
    void setAlignment(ParagraphAlignment align);

    // ========== 缩进相关方法（单位：毫米或点）==========
    
    /**
     * @brief 获取首行缩进
     * @return 首行缩进值
     */
    qreal firstLineIndent() const;
    
    /**
     * @brief 设置首行缩进
     * @param indent 新的首行缩进值
     */
    void setFirstLineIndent(qreal indent);

    /**
     * @brief 获取左缩进
     * @return 左缩进值
     */
    qreal leftIndent() const;
    
    /**
     * @brief 设置左缩进
     * @param indent 新的左缩进值
     */
    void setLeftIndent(qreal indent);

    /**
     * @brief 获取右缩进
     * @return 右缩进值
     */
    qreal rightIndent() const;
    
    /**
     * @brief 设置右缩进
     * @param indent 新的右缩进值
     */
    void setRightIndent(qreal indent);

    // ========== 间距相关方法（单位：毫米或点）==========
    
    /**
     * @brief 获取段前间距
     * @return 段前间距值
     */
    qreal spaceBefore() const;
    
    /**
     * @brief 设置段前间距
     * @param space 新的段前间距值
     */
    void setSpaceBefore(qreal space);

    /**
     * @brief 获取段后间距
     * @return 段后间距值
     */
    qreal spaceAfter() const;
    
    /**
     * @brief 设置段后间距
     * @param space 新的段后间距值
     */
    void setSpaceAfter(qreal space);

    // ========== 行高相关方法（百分比，例如150表示1.5倍行距）==========
    
    /**
     * @brief 获取行高百分比
     * @return 行高百分比值
     */
    int lineHeight() const;
    
    /**
     * @brief 设置行高百分比
     * @param percent 新的行高百分比（如150表示1.5倍行距）
     */
    void setLineHeight(int percent);

    // ========== 比较操作符 ==========
    
    /**
     * @brief 相等性比较
     * @param other 要比较的另一个段落样式
     * @return 如果两个样式相等返回true
     */
    bool operator==(const ParagraphStyle &other) const;
    
    /**
     * @brief 不等性比较
     * @param other 要比较的另一个段落样式
     * @return 如果两个样式不等返回true
     */
    bool operator!=(const ParagraphStyle &other) const;

    // ========== 重置方法 ==========
    
    /**
     * @brief 重置为默认样式
     * 将所有属性恢复为默认值
     */
    void reset();

private:
    QSharedDataPointer<ParagraphStyleData> d;  ///< 隐式共享数据指针
};

} // namespace QtWordEditor

#endif // PARAGRAPHSTYLE_H