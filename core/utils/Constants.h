#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QtGlobal>
#include <QColor>

namespace QtWordEditor {

namespace Constants {

// Default page dimensions (A4 portrait, in points)
constexpr qreal PAGE_WIDTH = 595.0;   // A4 width in points
constexpr qreal PAGE_HEIGHT = 842.0;  // A4 height in points
constexpr qreal PAGE_MARGIN = 72.0;   // 1 inch margin in points

// Default font
constexpr const char *DefaultFontFamily = "Times New Roman";
constexpr int DefaultFontSize = 12;

// Zoom range
constexpr qreal ZOOM_MIN = 0.5;
constexpr qreal ZOOM_MAX = 3.0;
constexpr qreal ZOOM_STEP = 0.1;

// Cursor settings
constexpr int CURSOR_BLINK_INTERVAL = 500; // milliseconds
constexpr qreal CURSOR_WIDTH = 2.0;

// Selection color
constexpr QRgb SELECTION_COLOR_RGB = 0xffadd8e6; // Light blue with transparency

// Unit conversion (millimeters to points, 1 mm = 2.83464567 pt)
constexpr qreal MillimetersToPoints = 2.83464567;

} // namespace Constants

} // namespace QtWordEditor

#endif // CONSTANTS_H