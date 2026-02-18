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
        , m_propertySetFlags()
    {
    }

    CharacterStyleData(const CharacterStyleData &other)
        : QSharedData(other)
        , m_font(other.m_font)
        , m_textColor(other.m_textColor)
        , m_backgroundColor(other.m_backgroundColor)
        , m_letterSpacing(other.m_letterSpacing)
        , m_propertySetFlags(other.m_propertySetFlags)
    {
    }

    ~CharacterStyleData() = default;

    QFont m_font;
    QColor m_textColor;
    QColor m_backgroundColor;
    qreal m_letterSpacing;
    CharacterStylePropertyFlags m_propertySetFlags;  ///< 属性设置标记
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
    if (d->m_font != font) {
        d->m_font = font;
        // 设置字体相关的所有属性标记
        d->m_propertySetFlags |= CharacterStyleProperty::FontFamily;
        d->m_propertySetFlags |= CharacterStyleProperty::FontSize;
        d->m_propertySetFlags |= CharacterStyleProperty::Bold;
        d->m_propertySetFlags |= CharacterStyleProperty::Italic;
        d->m_propertySetFlags |= CharacterStyleProperty::Underline;
        d->m_propertySetFlags |= CharacterStyleProperty::StrikeOut;
    }
}

QString CharacterStyle::fontFamily() const
{
    return d->m_font.family();
}

void CharacterStyle::setFontFamily(const QString &family)
{
    if (d->m_font.family() != family) {
        d->m_font.setFamily(family);
        d->m_propertySetFlags |= CharacterStyleProperty::FontFamily;
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
        d->m_propertySetFlags |= CharacterStyleProperty::FontSize;
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
        d->m_propertySetFlags |= CharacterStyleProperty::Bold;
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
        d->m_propertySetFlags |= CharacterStyleProperty::Italic;
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
        d->m_propertySetFlags |= CharacterStyleProperty::Underline;
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
        d->m_propertySetFlags |= CharacterStyleProperty::StrikeOut;
    }
}

QColor CharacterStyle::textColor() const
{
    return d->m_textColor;
}

void CharacterStyle::setTextColor(const QColor &color)
{
    if (d->m_textColor != color) {
        d->m_textColor = color;
        d->m_propertySetFlags |= CharacterStyleProperty::TextColor;
    }
}

QColor CharacterStyle::backgroundColor() const
{
    return d->m_backgroundColor;
}

void CharacterStyle::setBackgroundColor(const QColor &color)
{
    if (d->m_backgroundColor != color) {
        d->m_backgroundColor = color;
        d->m_propertySetFlags |= CharacterStyleProperty::BackgroundColor;
    }
}

qreal CharacterStyle::letterSpacing() const
{
    return d->m_letterSpacing;
}

void CharacterStyle::setLetterSpacing(qreal spacing)
{
    if (!qFuzzyCompare(d->m_letterSpacing, spacing)) {
        d->m_letterSpacing = spacing;
        d->m_propertySetFlags |= CharacterStyleProperty::LetterSpacing;
    }
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

bool CharacterStyle::isPropertySet(CharacterStyleProperty property) const
{
    return d->m_propertySetFlags.testFlag(property);
}

void CharacterStyle::clearProperty(CharacterStyleProperty property)
{
    // 清除属性标记
    d->m_propertySetFlags &= ~CharacterStylePropertyFlags(property);
    
    // 将属性恢复为默认值
    switch (property) {
        case CharacterStyleProperty::FontFamily:
            d->m_font.setFamily("Times New Roman");
            break;
        case CharacterStyleProperty::FontSize:
            d->m_font.setPointSize(12);
            break;
        case CharacterStyleProperty::Bold:
            d->m_font.setBold(false);
            break;
        case CharacterStyleProperty::Italic:
            d->m_font.setItalic(false);
            break;
        case CharacterStyleProperty::Underline:
            d->m_font.setUnderline(false);
            break;
        case CharacterStyleProperty::StrikeOut:
            d->m_font.setStrikeOut(false);
            break;
        case CharacterStyleProperty::TextColor:
            d->m_textColor = Qt::black;
            break;
        case CharacterStyleProperty::BackgroundColor:
            d->m_backgroundColor = Qt::transparent;
            break;
        case CharacterStyleProperty::LetterSpacing:
            d->m_letterSpacing = 0.0;
            break;
    }
}

void CharacterStyle::clearAllProperties()
{
    d = new CharacterStyleData;
}

CharacterStyle CharacterStyle::mergeWith(const CharacterStyle &other) const
{
    CharacterStyle result = *this;
    
    // 只合并另一个样式中已显式设置的属性
    if (other.isPropertySet(CharacterStyleProperty::FontFamily)) {
        result.setFontFamily(other.fontFamily());
    }
    if (other.isPropertySet(CharacterStyleProperty::FontSize)) {
        result.setFontSize(other.fontSize());
    }
    if (other.isPropertySet(CharacterStyleProperty::Bold)) {
        result.setBold(other.bold());
    }
    if (other.isPropertySet(CharacterStyleProperty::Italic)) {
        result.setItalic(other.italic());
    }
    if (other.isPropertySet(CharacterStyleProperty::Underline)) {
        result.setUnderline(other.underline());
    }
    if (other.isPropertySet(CharacterStyleProperty::StrikeOut)) {
        result.setStrikeOut(other.strikeOut());
    }
    if (other.isPropertySet(CharacterStyleProperty::TextColor)) {
        result.setTextColor(other.textColor());
    }
    if (other.isPropertySet(CharacterStyleProperty::BackgroundColor)) {
        result.setBackgroundColor(other.backgroundColor());
    }
    if (other.isPropertySet(CharacterStyleProperty::LetterSpacing)) {
        result.setLetterSpacing(other.letterSpacing());
    }
    
    return result;
}

} // namespace QtWordEditor