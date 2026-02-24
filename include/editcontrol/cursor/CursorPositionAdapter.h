
#ifndef CURSORPOSITIONADAPTER_H
#define CURSORPOSITIONADAPTER_H

#include "editcontrol/cursor/UnifiedCursor.h"

/**
 * @file CursorPositionAdapter.h
 * @brief 光标位置适配器
 * 
 * 在各种光标位置结构体之间进行转换
 * - CursorPosition <-> UnifiedCursorPosition
 */

namespace QtWordEditor {

/**
 * @brief 光标位置适配器
 * 
 * 在各种光标位置结构体之间进行转换
 */
class CursorPositionAdapter {
public:
    // ========== CursorPosition <-> UnifiedCursorPosition 转换 ==========
    
    /**
     * @brief 将 CursorPosition 转换为 UnifiedCursorPosition
     * @param cursorPos 旧格式的光标位置
     * @return 新格式的光标位置
     */
    static UnifiedCursorPosition cursorToUnified(const CursorPosition &cursorPos);
    
    /**
     * @brief 将 UnifiedCursorPosition 转换为 CursorPosition
     * @param unifiedPos 新格式的光标位置
     * @return 旧格式的光标位置
     */
    static CursorPosition unifiedToCursor(const UnifiedCursorPosition &unifiedPos);
};

} // namespace QtWordEditor

#endif // CURSORPOSITIONADAPTER_H

