#ifndef NUMBERMATHSPAN_H
#define NUMBERMATHSPAN_H

#include "core/document/MathSpan.h"
#include <QString>
#include "core/Global.h"

namespace QtWordEditor {

class NumberMathSpan : public MathSpan
{
    Q_OBJECT
public:
    NumberMathSpan(QObject *parent = nullptr);
    explicit NumberMathSpan(const QString &text, QObject *parent = nullptr);
    ~NumberMathSpan() override;

    MathType mathType() const override { return Number; }
    InlineSpan *clone() const override;

    QString text() const;
    void setText(const QString &text);
    void appendChar(QChar ch);
    void removeLastChar();
    int textLength() const;

signals:
    void textChanged(const QString &oldText, const QString &newText);

private:
    QString m_text;
};

} // namespace QtWordEditor

#endif // NUMBERMATHSPAN_H
