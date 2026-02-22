#include "core/document/math/NumberMathSpan.h"
#include <QDebug>

namespace QtWordEditor {

NumberMathSpan::NumberMathSpan(QObject *parent)
    : MathSpan(parent)
    , m_text("")
{
}

NumberMathSpan::NumberMathSpan(const QString &text, QObject *parent)
    : MathSpan(parent)
    , m_text(text)
{
}

NumberMathSpan::~NumberMathSpan()
{
}

QString NumberMathSpan::text() const
{
    return m_text;
}

void NumberMathSpan::setText(const QString &text)
{
    if (m_text != text) {
        QString oldText = m_text;
        m_text = text;
        emit textChanged(oldText, text);
        emit contentChanged();
    }
}

void NumberMathSpan::appendChar(QChar ch)
{
    QString oldText = m_text;
    m_text.append(ch);
    emit textChanged(oldText, m_text);
    emit contentChanged();
}

void NumberMathSpan::removeLastChar()
{
    if (!m_text.isEmpty()) {
        QString oldText = m_text;
        m_text.chop(1);
        emit textChanged(oldText, m_text);
        emit contentChanged();
    }
}

int NumberMathSpan::textLength() const
{
    return m_text.length();
}

InlineSpan *NumberMathSpan::clone() const
{
    NumberMathSpan *copy = new NumberMathSpan(m_text, nullptr);
    return copy;
}

} // namespace QtWordEditor
