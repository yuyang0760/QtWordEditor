#ifndef INLINESPAN_H
#define INLINESPAN_H

#include <QString>
#include <QPointF>
#include <QPainter>
#include "core/Global.h"

namespace QtWordEditor {

/**
 * @brief 所有内联内容的基类（文本和公式）
 * 
 * 这是内联内容系统的基石。TextSpan和MathSpan都继承自此类，
 * 使得文本和公式可以在ParagraphBlock中统一管理。
 */
class InlineSpan
{
public:
    /**
     * @brief 内联内容类型
     */
    enum Type {
        Text,    ///< 文本
        Math     ///< 公式
    };
    
    /**
     * @brief 构造函数
     */
    InlineSpan();
    
    /**
     * @brief 析构函数
     */
    virtual ~InlineSpan() = default;

    // ========== 基本信息 ==========
    
    /**
     * @brief 获取内联内容类型
     * @return Type枚举值
     */
    virtual Type type() const = 0;
    
    /**
     * @brief 获取占用的字符数
     * @return 字符数（TextSpan返回文本长度，MathSpan返回1）
     */
    virtual int length() const = 0;

    // ========== 克隆 ==========
    
    /**
     * @brief 克隆当前内联内容
     * @return 新的InlineSpan指针
     */
    virtual InlineSpan *clone() const = 0;
};

} // namespace QtWordEditor

#endif // INLINESPAN_H
