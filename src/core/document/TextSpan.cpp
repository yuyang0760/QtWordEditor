#include "core/document/TextSpan.h"
#include "core/styles/StyleManager.h"

namespace QtWordEditor {

TextSpan::TextSpan(QObject *parent)
    : InlineSpan(parent)
    , m_text("")
    , m_styleName("")
{
    updateFont();
}

TextSpan::TextSpan(const QString &text, QObject *parent)
    : InlineSpan(parent)
    , m_text(text)
    , m_styleName("")
{
    updateFont();
}

TextSpan::TextSpan(const QString &text, const CharacterStyle &style, QObject *parent)
    : InlineSpan(parent)
    , m_text(text)
    , m_styleName("")
    , m_directStyle(style)
{
    updateFont();
}

TextSpan::TextSpan(const TextSpan &other, QObject *parent)
    : InlineSpan(parent)
    , m_text(other.m_text)
    , m_styleName(other.m_styleName)
    , m_directStyle(other.m_directStyle)
    , m_font(other.m_font)
{
}

TextSpan::~TextSpan()
{
}

int TextSpan::length() const
{
    return m_text.length();
}

InlineSpan *TextSpan::clone() const
{
    return new TextSpan(*this, nullptr);
}

QString TextSpan::text() const
{
    return m_text;
}

void TextSpan::setText(const QString &text)
{
    m_text = text;
    emit textChanged();
}

void TextSpan::append(const QString &text)
{
    m_text.append(text);
    emit textChanged();
}

void TextSpan::insert(int position, const QString &text)
{
    m_text.insert(position, text);
    emit textChanged();
}

void TextSpan::remove(int position, int length)
{
    m_text.remove(position, length);
    emit textChanged();
}

CharacterStyle TextSpan::style() const
{
    return m_directStyle;
}

void TextSpan::setStyle(const CharacterStyle &style)
{
    m_directStyle = style;
    updateFont();
}

QString TextSpan::styleName() const
{
    return m_styleName;
}

void TextSpan::setStyleName(const QString &styleName)
{
    m_styleName = styleName;
    updateFont();
}

CharacterStyle TextSpan::directStyle() const
{
    return m_directStyle;
}

void TextSpan::setDirectStyle(const CharacterStyle &style)
{
    m_directStyle = style;
    updateFont();
}

CharacterStyle TextSpan::effectiveStyle(const StyleManager *styleManager) const
{
    CharacterStyle result;
    
    if (!m_styleName.isEmpty() && styleManager) {
        if (styleManager->hasCharacterStyle(m_styleName)) {
            result = styleManager->getResolvedCharacterStyle(m_styleName);
        }
    }
    
    result = result.mergeWith(m_directStyle);
    
    return result;
}

TextSpan *TextSpan::split(int position)
{
    if (position <= 0 || position >= m_text.length()) {
        return new TextSpan(*this, nullptr);
    }
    
    QString firstPart = m_text.left(position);
    QString secondPart = m_text.mid(position);
    
    TextSpan *second = new TextSpan(secondPart, nullptr);
    second->setStyleName(m_styleName);
    second->setDirectStyle(m_directStyle);
    
    m_text = firstPart;
    
    return second;
}

bool TextSpan::operator==(const TextSpan &other) const
{
    return m_text == other.m_text && 
           m_styleName == other.m_styleName &&
           m_directStyle == other.m_directStyle;
}

bool TextSpan::operator!=(const TextSpan &other) const
{
    return !(*this == other);
}

QFont TextSpan::font() const
{
    return m_font;
}

void TextSpan::updateFont()
{
    m_font = m_directStyle.font();
}

} // namespace QtWordEditor
