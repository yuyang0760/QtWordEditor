#ifndef SPAN_H
#define SPAN_H

#include <QString>
#include <QSharedData>
#include <QSharedDataPointer>
#include "CharacterStyle.h"
#include "core/Global.h"

namespace QtWordEditor {

class SpanData;

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

    // Style
    CharacterStyle style() const;
    void setStyle(const CharacterStyle &style);

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