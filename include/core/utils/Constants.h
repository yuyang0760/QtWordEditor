#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QtGlobal>
#include <QColor>
#include "core/document/ParagraphStyle.h"

namespace QtWordEditor {

namespace Constants {

// ==========================================
// 页面尺寸相关常量
// ==========================================
// 默认页面尺寸 (A4 纵向，单位：点)
constexpr qreal PAGE_WIDTH = 595.0;   // A4 纸宽度 (210mm)
constexpr qreal PAGE_HEIGHT = 842.0;  // A4 纸高度 (297mm)
constexpr qreal PAGE_MARGIN = 72.0;   // 页边距 (1 英寸 = 72 点)

// 页面尺寸预设 (单位：毫米)
// A4: 210 × 297 mm
constexpr qreal PAGE_SIZE_A4_WIDTH = 210.0;
constexpr qreal PAGE_SIZE_A4_HEIGHT = 297.0;
// A3: 297 × 420 mm
constexpr qreal PAGE_SIZE_A3_WIDTH = 297.0;
constexpr qreal PAGE_SIZE_A3_HEIGHT = 420.0;
// A5: 148 × 210 mm
constexpr qreal PAGE_SIZE_A5_WIDTH = 148.0;
constexpr qreal PAGE_SIZE_A5_HEIGHT = 210.0;
// Letter: 215.9 × 279.4 mm (8.5 × 11 英寸)
constexpr qreal PAGE_SIZE_LETTER_WIDTH = 215.9;
constexpr qreal PAGE_SIZE_LETTER_HEIGHT = 279.4;
// Legal: 215.9 × 355.6 mm (8.5 × 14 英寸)
constexpr qreal PAGE_SIZE_LEGAL_WIDTH = 215.9;
constexpr qreal PAGE_SIZE_LEGAL_HEIGHT = 355.6;
// Tabloid: 279.4 × 431.8 mm (11 × 17 英寸)
constexpr qreal PAGE_SIZE_TABLOID_WIDTH = 279.4;
constexpr qreal PAGE_SIZE_TABLOID_HEIGHT = 431.8;

// 边距预设 (单位：毫米)
// Normal: 20 mm
constexpr qreal MARGIN_NORMAL = 20.0;
// Narrow: 12.7 mm (0.5 英寸)
constexpr qreal MARGIN_NARROW = 12.7;
// Moderate: 25.4 mm (1 英寸)
constexpr qreal MARGIN_MODERATE = 25.4;
// Wide: 50.8 mm (2 英寸)
constexpr qreal MARGIN_WIDE = 50.8;

// 页面尺寸控件范围
constexpr qreal PAGE_SIZE_MIN = 50.0;
constexpr qreal PAGE_SIZE_MAX = 500.0;
constexpr int PAGE_SIZE_DECIMALS = 1;
constexpr qreal PAGE_SIZE_STEP = 1.0;

// 边距控件范围
constexpr qreal MARGIN_MIN = 0.0;
constexpr qreal MARGIN_MAX = 100.0;
constexpr int MARGIN_DECIMALS = 1;
constexpr qreal MARGIN_STEP = 1.0;

// ==========================================
// 界面布局相关常量
// ==========================================
// RibbonBar 固定高度
constexpr int RIBBON_BAR_HEIGHT = 140;

// 页面与 RibbonBar 之间的间距
constexpr int PAGE_TOP_SPACING = 20;

// ==========================================
// 段落对齐方式常量
// ==========================================
// 默认对齐方式
constexpr ParagraphAlignment DEFAULT_ALIGNMENT = ParagraphAlignment::AlignLeft;

// ==========================================
// 段落样式相关常量
// ==========================================
// 默认段前间距
constexpr qreal DEFAULT_SPACE_BEFORE = 6.0;
// 默认段后间距
constexpr qreal DEFAULT_SPACE_AFTER = 6.0;
// 默认行高百分比 (100% = 1倍行距)
constexpr int DEFAULT_LINE_HEIGHT_PERCENT = 100;
// 默认首行缩进 (2个字符，约 24 点，以 12pt 字体计算)
constexpr qreal DEFAULT_FIRST_LINE_INDENT = 24.0;
// 默认左缩进
constexpr qreal DEFAULT_LEFT_INDENT = 0.0;
// 默认右缩进
constexpr qreal DEFAULT_RIGHT_INDENT = 0.0;
// 悬挂缩进预设值 (36 点 = 0.5 英寸)
constexpr qreal HANGING_INDENT_VALUE = 36.0;

// ==========================================
// 字体相关常量
// ==========================================
// 默认字体
// 注意：不再硬编码 "Times New Roman"，让 CharacterStyle 使用系统默认字体
// 只保留 DefaultFontSize 作为默认字号
constexpr int DefaultFontSize = 12;

// ==========================================
// 缩放相关常量
// ==========================================
// 缩放范围
constexpr qreal ZOOM_MIN = 0.5;
constexpr qreal ZOOM_MAX = 3.0;
constexpr qreal ZOOM_STEP = 0.1;

// ==========================================
// 光标相关常量
// ==========================================
// 光标设置
constexpr int CURSOR_BLINK_INTERVAL = 500; // 光标闪烁间隔 (毫秒)
constexpr qreal CURSOR_WIDTH = 2.0;         // 光标宽度

// ==========================================
// 选择相关常量
// ==========================================
// 选择颜色 (浅蓝色，带透明度)
constexpr QRgb SELECTION_COLOR_RGB = 0xffadd8e6;

// ==========================================
// 单位转换相关常量
// ==========================================
// 毫米转点 (1 毫米 = 2.83464567 点)
constexpr qreal MillimetersToPoints = 2.83464567;

} // namespace Constants

} // namespace QtWordEditor

#endif // CONSTANTS_H