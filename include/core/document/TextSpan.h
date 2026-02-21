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
 * 替换原来的 Span 类，但保留相似的接口。
 */
class TextSpan : public InlineSpan
{
public:
    explicit TextSpan(QObject *parent = nullptr);
    explicit TextSpan(const QString &text, QObject *parent = nullptr);
    TextSpan(const QString &text, const CharacterStyle &style, QObject *parent = nullptr);
    ~TextSpan() override;

    // ========== InlineSpan 接口 ==========
    Type type() const override { return Text; }
    int length() const override;
    InlineSpan *clone() const override;

    // ========== 文本内容 ==========
    QString text() const;
    void setText(const QString &text);
    void append(const QString &text);
    void insert(int position, const QString &text);
    void remove(int position, int length);

    // ========== 样式 - 传统接口（向后兼容） ==========
    CharacterStyle style() const;
    void setStyle(const CharacterStyle &style);

    // ========== 命名样式 ==========
    QString styleName() const;
    void setStyleName(const QString &styleName);

    // ========== 直接样式（覆盖命名样式） ==========
    CharacterStyle directStyle() const;
    void setDirectStyle(const CharacterStyle &style);

    // ========== 获取最终生效的样式（命名样式 + 直接样式） ==========
    CharacterStyle effectiveStyle(const StyleManager *styleManager) const;

    // ========== 字体 ==========
    QFont font() const;

    // ========== 比较操作符 ==========
    bool operator==(const TextSpan &other) const;
    bool operator!=(const TextSpan &other) const;

private:
    QString m_text;               // 文本内容
    QString m_styleName;       // 命名样式名称
    CharacterStyle m_directStyle; // 直接样式（覆盖命名样式）
};

} // namespace QtWordEditor

#endif // TEXTSPAN_H
