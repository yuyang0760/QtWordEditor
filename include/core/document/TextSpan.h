#ifndef TEXTSPAN_H
#define TEXTSPAN_H

#include "InlineSpan.h"
#include "CharacterStyle.h"
#include <QString>
#include <QFont>
#include "core/Global.h"

namespace QtWordEditor {

class StyleManager;

class TextSpan : public InlineSpan
{
    Q_OBJECT
public:
    TextSpan(QObject *parent = nullptr);
    explicit TextSpan(const QString &text, QObject *parent = nullptr);
    TextSpan(const QString &text, const CharacterStyle &style, QObject *parent = nullptr);
    TextSpan(const TextSpan &other, QObject *parent = nullptr);
    ~TextSpan() override;

    Type type() const override { return Text; }
    int length() const override;
    InlineSpan *clone() const override;

    // ========== 文本操作 ==========
    QString text() const;
    void setText(const QString &text);
    void append(const QString &text);
    void insert(int position, const QString &text);
    void remove(int position, int length);

    // ========== 样式管理 ==========
    CharacterStyle style() const;
    void setStyle(const CharacterStyle &style);
    QString styleName() const;
    void setStyleName(const QString &styleName);
    CharacterStyle directStyle() const;
    void setDirectStyle(const CharacterStyle &style);
    CharacterStyle effectiveStyle(const StyleManager *styleManager) const;

    // ========== 分割操作 ==========
    TextSpan *split(int position);

    // ========== 比较操作 ==========
    bool operator==(const TextSpan &other) const;
    bool operator!=(const TextSpan &other) const;

    QFont font() const;

signals:
    void textChanged();

private:
    QString m_text;
    QString m_styleName;
    CharacterStyle m_directStyle;
    QFont m_font;

    void updateFont();
};

} // namespace QtWordEditor

#endif // TEXTSPAN_H
