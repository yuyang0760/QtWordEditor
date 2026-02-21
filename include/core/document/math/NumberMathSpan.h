/**
 * @file NumberMathSpan.h
 * @brief 数字/变量数据类
 * 
 * 用于表示公式中的数字、变量、标识符等文本内容。
 */

#ifndef NUMBERMATHSPAN_H
#define NUMBERMATHSPAN_H

#include "core/document/MathSpan.h"
#include <QString>
#include "core/Global.h"

namespace QtWordEditor {

/**
 * @brief 数字/变量数据类
 * 
 * 用于表示公式中的数字、变量、标识符等文本内容。
 */
class NumberMathSpan : public MathSpan
{
    Q_OBJECT
public:
    /**
     * @brief 默认构造函数
     */
    explicit NumberMathSpan(QObject *parent = nullptr);

    /**
     * @brief 带文本的构造函数
     * @param text 初始文本内容
     */
    explicit NumberMathSpan(const QString &text, QObject *parent = nullptr);

    /**
     * @brief 析构函数
     */
    ~NumberMathSpan() override;

    // ========== MathSpan 接口实现 ==========

    /**
     * @brief 获取公式元素类型
     * @return 固定返回 MathSpan::Number
     */
    MathType mathType() const override { return Number; }

    /**
     * @brief 克隆当前对象
     * @return 新的 NumberMathSpan 指针
     */
    InlineSpan *clone() const override;

    // ========== NumberMathSpan 特有接口 ==========

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
     * @brief 追加一个字符
     * @param ch 要追加的字符
     */
    void appendChar(QChar ch);

    /**
     * @brief 删除最后一个字符
     */
    void removeLastChar();

    /**
     * @brief 获取文本长度
     * @return 文本字符数
     */
    int textLength() const;

signals:
    /**
     * @brief 文本内容变化信号
     * @param oldText 旧的文本内容
     * @param newText 新的文本内容
     */
    void textChanged(const QString &oldText, const QString &newText);

private:
    QString m_text;  ///< 存储的文本内容
};

} // namespace QtWordEditor

#endif // NUMBERMATHSPAN_H
