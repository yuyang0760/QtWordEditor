#include "core/utils/FontUtils.h"
#include <QFontMetricsF>
#include <QScreen>
#include <QGuiApplication>

namespace QtWordEditor {

qreal FontUtils::pointsToPixels(qreal points, qreal dpi)
{
    return points * dpi / 72.0;
}

qreal FontUtils::pixelsToPoints(qreal pixels, qreal dpi)
{
    return pixels * 72.0 / dpi;
}

qreal FontUtils::lineHeight(const QFont &font, int percent, qreal dpi)
{
    qreal height = textHeight(font, dpi);
    return height * percent / 100.0;
}

qreal FontUtils::textWidth(const QFont &font, const QString &text, qreal dpi)
{
    QFontMetricsF fm(font);
    // Adjust for DPI?
    // In practice, QFontMetrics already works with the screen's logical DPI.
    // We'll just return the raw pixel width.
    return fm.horizontalAdvance(text);
}

qreal FontUtils::textHeight(const QFont &font, qreal dpi)
{
    Q_UNUSED(dpi);
    QFontMetricsF fm(font);
    return fm.height();
}

} // namespace QtWordEditor