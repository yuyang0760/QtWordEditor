#include "core/document/TextSpan.h"
#include "core/styles/StyleManager.h"

namespace QtWordEditor {

TextSpan::TextSpan()
    : InlineSpan()
    , m_text("")
    , m_styleName("")
    , m_directStyle(CharacterStyle())
{
}

TextSpan::TextSpan(const QString &text)
    : InlineSpan()
    , m_text(text)
    , m_styleName("")
    , m_directStyle(CharacterStyle())
{
}

TextSpan::TextSpan(const QString &text, const CharacterStyle &style)
    : InlineSpan()
    , m_text(text)
    , m_styleName("")
    , m_directStyle(style)
{
}

TextSpan::TextSpan(const TextSpan &other)
    : InlineSpan(other)
    , m_text(other.m_text)
    , m_styleName(other.m_styleName)
    , m_directStyle(other.m_directStyle)
{
}

TextSpan &TextSpan::operator=(const TextSpan &other)
{
    if (this != &other) {
        m_text = other.m_text;
        m_styleName = other.m_styleName;
        m_directStyle = other.m_directStyle;
    }
    return *this;
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
    return new TextSpan(*this);
}

QString TextSpan::text() const
{
    return m_text;
}

void TextSpan::setText(const QString &text)
{
    m_text = text;
}

void TextSpan::append(const QString &text)
{
    m_text.append(text);
}

void TextSpan::insert(int position, const QString &text)
{
    m_text.insert(position, text);
}

void TextSpan::remove(int position, int length)
{
    m_text.remove(position, length);
}

CharacterStyle TextSpan::style() const
{
    // 向后兼容：返回直接样式
    return m_directStyle;
}

void TextSpan::setStyle(const CharacterStyle &style)
{
    // 向后兼容：设置直接样式
    m_directStyle = style;
}

QString TextSpan::styleName() const
{
    return m_styleName;
}

void TextSpan::setStyleName(const QString &styleName)
{
    m_styleName = styleName;
}

CharacterStyle TextSpan::directStyle() const
{
    return m_directStyle;
}

void TextSpan::setDirectStyle(const CharacterStyle &style)
{
    m_directStyle = style;
}

CharacterStyle TextSpan::effectiveStyle(const StyleManager *styleManager) const
{
    CharacterStyle result;
    
    // 1. 如果有命名样式，先应用（解析继承）
    if (!m_styleName.isEmpty() && styleManager) {
        if (styleManager->hasCharacterStyle(m_styleName)) {
            result = styleManager->getResolvedCharacterStyle(m_styleName);
        }
    }
    
    // 2. 然后用直接样式覆盖
    result = result.mergeWith(m_directStyle);
    
    return result;
}

QFont TextSpan::font() const
{
    return m_directStyle.font();
}

TextSpan TextSpan::split(int position)
{
    if (position <= 0 || position >= m_text.length()) {
        return TextSpan(*this);
    }
    QString secondPart = m_text.mid(position);
    m_text = m_text.left(position);
    TextSpan second(secondPart);
    second.setStyleName(m_styleName);
    second.setDirectStyle(m_directStyle);
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

} // namespace QtWordEditor
