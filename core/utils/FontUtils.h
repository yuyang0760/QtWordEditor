#ifndef FONTUTILS_H
#define FONTUTILS_H

#include <QFont>
#include <QFontMetricsF>
#include "core/Global.h"

namespace QtWordEditor {

class FontUtils
{
public:
    // Convert font size from points to pixels for a given screen DPI.
    static qreal pointsToPixels(qreal points, qreal dpi = 96.0);

    // Convert font size from pixels to points.
    static qreal pixelsToPoints(qreal pixels, qreal dpi = 96.0);

    // Calculate line height in pixels for a given font and line‑height percentage.
    static qreal lineHeight(const QFont &font, int percent, qreal dpi = 96.0);

    // Calculate the width of a text string in pixels.
    static qreal textWidth(const QFont &font, const QString &text, qreal dpi = 96.0);

    // Calculate the height of a single line of text (ascent + descent).
    static qreal textHeight(const QFont &font, qreal dpi = 96.0);
};

} // namespace QtWordEditor

#endif // FONTUTILS_H