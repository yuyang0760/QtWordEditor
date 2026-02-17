#ifndef CHARACTERSTYLE_H
#define CHARACTERSTYLE_H

#include <QFont>
#include <QColor>
#include <QSharedData>
#include <QSharedDataPointer>
#include "core/Global.h"

namespace QtWordEditor {

class CharacterStyleData;

/**
 * @brief The CharacterStyle class encapsulates character-level formatting attributes.
 *
 * It uses implicit sharing (copy-on-write) for efficient copying.
 */
class CharacterStyle
{
public:
    CharacterStyle();
    CharacterStyle(const CharacterStyle &other);
    CharacterStyle &operator=(const CharacterStyle &other);
    ~CharacterStyle();

    // Font properties
    QFont font() const;
    void setFont(const QFont &font);

    QString fontFamily() const;
    void setFontFamily(const QString &family);

    int fontSize() const;
    void setFontSize(int size);

    bool bold() const;
    void setBold(bool bold);

    bool italic() const;
    void setItalic(bool italic);

    bool underline() const;
    void setUnderline(bool underline);

    bool strikeOut() const;
    void setStrikeOut(bool strikeOut);

    // Colors
    QColor textColor() const;
    void setTextColor(const QColor &color);

    QColor backgroundColor() const;
    void setBackgroundColor(const QColor &color);

    // Letter spacing
    qreal letterSpacing() const;
    void setLetterSpacing(qreal spacing);

    // Comparison
    bool operator==(const CharacterStyle &other) const;
    bool operator!=(const CharacterStyle &other) const;

    // Reset to default
    void reset();

private:
    QSharedDataPointer<CharacterStyleData> d;
};

} // namespace QtWordEditor

#endif // CHARACTERSTYLE_H