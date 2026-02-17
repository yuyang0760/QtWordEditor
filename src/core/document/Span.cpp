#include "include/core/document/Span.h"
#include <QDebug>

namespace QtWordEditor {

class SpanData : public QSharedData
{
public:
    SpanData()
        : m_text("")
    {
    }

    SpanData(const QString &text, const CharacterStyle &style)
        : m_text(text)
        , m_style(style)
    {
    }

    SpanData(const SpanData &other)
        : QSharedData(other)
        , m_text(other.m_text)
        , m_style(other.m_style)
    {
    }

    ~SpanData() = default;

    QString m_text;
    CharacterStyle m_style;
};

Span::Span()
    : d(new SpanData)
{
}

Span::Span(const QString &text)
    : d(new SpanData(text, CharacterStyle()))
{
}

Span::Span(const QString &text, const CharacterStyle &style)
    : d(new SpanData(text, style))
{
}

Span::Span(const Span &other)
    : d(other.d)
{
}

Span &Span::operator=(const Span &other)
{
    if (this != &other)
        d = other.d;
    return *this;
}

Span::~Span()
{
}

QString Span::text() const
{
    return d->m_text;
}

void Span::setText(const QString &text)
{
    d->m_text = text;
}

void Span::append(const QString &text)
{
    d->m_text.append(text);
}

void Span::insert(int position, const QString &text)
{
    d->m_text.insert(position, text);
}

void Span::remove(int position, int length)
{
    d->m_text.remove(position, length);
}

int Span::length() const
{
    return d->m_text.length();
}

CharacterStyle Span::style() const
{
    return d->m_style;
}

void Span::setStyle(const CharacterStyle &style)
{
    d->m_style = style;
}

Span Span::split(int position)
{
    if (position <= 0 || position >= d->m_text.length()) {
        return Span(*this);
    }
    QString firstPart = d->m_text.left(position);
    QString secondPart = d->m_text.mid(position);
    Span second(secondPart, d->m_style);
    d->m_text = firstPart;
    return second;
}

bool Span::operator==(const Span &other) const
{
    return d->m_text == other.d->m_text && d->m_style == other.d->m_style;
}

bool Span::operator!=(const Span &other) const
{
    return !(*this == other);
}

} // namespace QtWordEditor