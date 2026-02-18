#include "core/document/Span.h"
#include "core/styles/StyleManager.h"
#include <QDebug>

namespace QtWordEditor {

class SpanData : public QSharedData
{
public:
    SpanData()
        : m_text("")
        , m_styleName("")
    {
    }

    SpanData(const QString &text, const CharacterStyle &style)
        : m_text(text)
        , m_styleName("")
        , m_directStyle(style)
    {
    }

    SpanData(const SpanData &other)
        : QSharedData(other)
        , m_text(other.m_text)
        , m_styleName(other.m_styleName)
        , m_directStyle(other.m_directStyle)
    {
    }

    ~SpanData() = default;

    QString m_text;
    QString m_styleName;           // 命名样式名称
    CharacterStyle m_directStyle;  // 直接样式（覆盖命名样式）
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
    // 向后兼容：返回直接样式
    return d->m_directStyle;
}

void Span::setStyle(const CharacterStyle &style)
{
    // 向后兼容：设置直接样式
    d->m_directStyle = style;
}

QString Span::styleName() const
{
    return d->m_styleName;
}

void Span::setStyleName(const QString &styleName)
{
    d->m_styleName = styleName;
}

CharacterStyle Span::directStyle() const
{
    return d->m_directStyle;
}

void Span::setDirectStyle(const CharacterStyle &style)
{
    d->m_directStyle = style;
}

CharacterStyle Span::effectiveStyle(const StyleManager *styleManager) const
{
    CharacterStyle result;
    
    // 1. 如果有命名样式，先应用（解析继承）
    if (!d->m_styleName.isEmpty() && styleManager) {
        if (styleManager->hasCharacterStyle(d->m_styleName)) {
            result = styleManager->getResolvedCharacterStyle(d->m_styleName);
        }
    }
    
    // 2. 然后用直接样式覆盖
    result = result.mergeWith(d->m_directStyle);
    
    return result;
}

Span Span::split(int position)
{
    if (position <= 0 || position >= d->m_text.length()) {
        return Span(*this);
    }
    QString firstPart = d->m_text.left(position);
    QString secondPart = d->m_text.mid(position);
    Span second(secondPart);
    second.setStyleName(d->m_styleName);
    second.setDirectStyle(d->m_directStyle);
    d->m_text = firstPart;
    return second;
}

bool Span::operator==(const Span &other) const
{
    return d->m_text == other.d->m_text && 
           d->m_styleName == other.d->m_styleName &&
           d->m_directStyle == other.d->m_directStyle;
}

bool Span::operator!=(const Span &other) const
{
    return !(*this == other);
}

} // namespace QtWordEditor
