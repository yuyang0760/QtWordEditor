#include "core/document/TextSpan.h"
#include "core/styles/StyleManager.h"

namespace QtWordEditor {

TextSpan::TextSpan(QObject *parent)
    : InlineSpan(parent)
    , m_text("")
    , m_styleName("")
    , m_directStyle(CharacterStyle())
{
}

TextSpan::TextSpan(const QString &text, QObject *parent)
    : InlineSpan(parent)
    , m_text(text)
    , m_styleName("")
    , m_directStyle(CharacterStyle())
{
}

TextSpan::TextSpan(const QString &text, const CharacterStyle &style, QObject *parent)
    : InlineSpan(parent)
    , m_text(text)
    , m_styleName("")
    , m_directStyle(style)
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
    TextSpan *cloned = new TextSpan();
    cloned->m_text = m_text;
    cloned->m_styleName = m_styleName;
    cloned->m_directStyle = m_directStyle;
    return cloned;
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
