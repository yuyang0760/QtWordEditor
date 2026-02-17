#include "core/document/CharacterStyle.h"
#include <QDebug>

namespace QtWordEditor {

class CharacterStyleData : public QSharedData
{
public:
    CharacterStyleData()
        : m_font("Times New Roman", 12)
        , m_textColor(Qt::black)
        , m_backgroundColor(Qt::transparent)
        , m_letterSpacing(0.0)
    {
    }

    CharacterStyleData(const CharacterStyleData &other)
        : QSharedData(other)
        , m_font(other.m_font)
        , m_textColor(other.m_textColor)
        , m_backgroundColor(other.m_backgroundColor)
        , m_letterSpacing(other.m_letterSpacing)
    {
    }

    ~CharacterStyleData() = default;

    QFont m_font;
    QColor m_textColor;
    QColor m_backgroundColor;
    qreal m_letterSpacing;
};

CharacterStyle::CharacterStyle()
    : d(new CharacterStyleData)
{
}

CharacterStyle::CharacterStyle(const CharacterStyle &other)
    : d(other.d)
{
}

CharacterStyle &CharacterStyle::operator=(const CharacterStyle &other)
{
    if (this != &other)
        d = other.d;
    return *this;
}

CharacterStyle::~CharacterStyle()
{
}

QFont CharacterStyle::font() const
{
    return d->m_font;
}

void CharacterStyle::setFont(const QFont &font)
{
    d->m_font = font;
}

QString CharacterStyle::fontFamily() const
{
    return d->m_font.family();
}

void CharacterStyle::setFontFamily(const QString &family)
{
    if (d->m_font.family() != family) {
        d->m_font.setFamily(family);
    }
}

int CharacterStyle::fontSize() const
{
    return d->m_font.pointSize();
}

void CharacterStyle::setFontSize(int size)
{
    if (d->m_font.pointSize() != size) {
        d->m_font.setPointSize(size);
    }
}

bool CharacterStyle::bold() const
{
    return d->m_font.bold();
}

void CharacterStyle::setBold(bool bold)
{
    if (d->m_font.bold() != bold) {
        d->m_font.setBold(bold);
    }
}

bool CharacterStyle::italic() const
{
    return d->m_font.italic();
}

void CharacterStyle::setItalic(bool italic)
{
    if (d->m_font.italic() != italic) {
        d->m_font.setItalic(italic);
    }
}

bool CharacterStyle::underline() const
{
    return d->m_font.underline();
}

void CharacterStyle::setUnderline(bool underline)
{
    if (d->m_font.underline() != underline) {
        d->m_font.setUnderline(underline);
    }
}

bool CharacterStyle::strikeOut() const
{
    return d->m_font.strikeOut();
}

void CharacterStyle::setStrikeOut(bool strikeOut)
{
    if (d->m_font.strikeOut() != strikeOut) {
        d->m_font.setStrikeOut(strikeOut);
    }
}

QColor CharacterStyle::textColor() const
{
    return d->m_textColor;
}

void CharacterStyle::setTextColor(const QColor &color)
{
    d->m_textColor = color;
}

QColor CharacterStyle::backgroundColor() const
{
    return d->m_backgroundColor;
}

void CharacterStyle::setBackgroundColor(const QColor &color)
{
    d->m_backgroundColor = color;
}

qreal CharacterStyle::letterSpacing() const
{
    return d->m_letterSpacing;
}

void CharacterStyle::setLetterSpacing(qreal spacing)
{
    d->m_letterSpacing = spacing;
}

bool CharacterStyle::operator==(const CharacterStyle &other) const
{
    return d->m_font == other.d->m_font &&
           d->m_textColor == other.d->m_textColor &&
           d->m_backgroundColor == other.d->m_backgroundColor &&
           qFuzzyCompare(d->m_letterSpacing, other.d->m_letterSpacing);
}

bool CharacterStyle::operator!=(const CharacterStyle &other) const
{
    return !(*this == other);
}

void CharacterStyle::reset()
{
    d = new CharacterStyleData;
}

} // namespace QtWordEditor