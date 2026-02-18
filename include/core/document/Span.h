#ifndef SPAN_H
#define SPAN_H

#include <QString>
#include <QSharedData>
#include <QSharedDataPointer>
#include "CharacterStyle.h"
#include "core/Global.h"

namespace QtWordEditor {

class SpanData;
class StyleManager;

/**
 * @brief The Span class represents a text fragment with uniform character style.
 */
class Span
{
public:
    Span();
    Span(const QString &text);
    Span(const QString &text, const CharacterStyle &style);
    Span(const Span &other);
    Span &operator=(const Span &other);
    ~Span();

    // Text content
    QString text() const;
    void setText(const QString &text);
    void append(const QString &text);
    void insert(int position, const QString &text);
    void remove(int position, int length);
    int length() const;

    // Style - 传统接口（向后兼容）
    CharacterStyle style() const;
    void setStyle(const CharacterStyle &style);

    // 命名样式
    QString styleName() const;
    void setStyleName(const QString &styleName);

    // 直接样式（覆盖命名样式）
    CharacterStyle directStyle() const;
    void setDirectStyle(const CharacterStyle &style);

    // 获取最终生效的样式（命名样式 + 直接样式）
    CharacterStyle effectiveStyle(const StyleManager *styleManager) const;

    // Split at position, returning the second part; this span becomes the first part.
    Span split(int position);

    // Comparison
    bool operator==(const Span &other) const;
    bool operator!=(const Span &other) const;

private:
    QSharedDataPointer<SpanData> d;
};

} // namespace QtWordEditor

#endif // SPAN_H