#ifndef CURSORPOSITIONADAPTER_H
#define CURSORPOSITIONADAPTER_H

#include "editcontrol/cursor/UnifiedCursor.h"
#include "editcontrol/cursor/NewCursorPosition.h"

/**
 * @file CursorPositionAdapter.h
 * @brief 光标位置适配器
 * 
 * 在 UnifiedCursorPosition 和 NewCursorPosition 之间进行转换
 * 由于现在 UnifiedCursorPosition 已简化为无模式版本，
 * 转换变得非常简单，主要是字段的直接复制
 */

namespace QtWordEditor {

/**
 * @brief 光标位置适配器
 * 
 * 在 UnifiedCursorPosition 和 NewCursorPosition 之间进行转换
 */
class CursorPositionAdapter {
public:
    /**
     * @brief 将 UnifiedCursorPosition 转换为 NewCursorPosition
     * @param oldPos 光标位置
     * @return 新的光标位置
     */
    static NewCursorPosition toNew(const UnifiedCursorPosition& oldPos);
    
    /**
     * @brief 将 NewCursorPosition 转换为 UnifiedCursorPosition
     * @param newPos 新的光标位置
     * @return 光标位置
     */
    static UnifiedCursorPosition toOld(const NewCursorPosition& newPos);
};

} // namespace QtWordEditor

#endif // CURSORPOSITIONADAPTER_H
