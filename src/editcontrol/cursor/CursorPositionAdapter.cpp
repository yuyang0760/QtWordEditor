/**
 * @file CursorPositionAdapter.cpp
 * @brief 光标位置适配器实现
 * 
 * 在 UnifiedCursorPosition 和 NewCursorPosition 之间进行转换
 * 由于现在 UnifiedCursorPosition 已简化为无模式版本，
 * 转换变得非常简单，主要是字段的直接复制
 */

#include "editcontrol/cursor/CursorPositionAdapter.h"

namespace QtWordEditor {

/**
 * @brief 将 UnifiedCursorPosition 转换为 NewCursorPosition
 * @param oldPos 光标位置
 * @return 新的光标位置
 */
NewCursorPosition CursorPositionAdapter::toNew(const UnifiedCursorPosition& oldPos) {
    NewCursorPosition newPos;
    
    // 直接复制字段
    newPos.blockIndex = oldPos.blockIndex;
    newPos.offset = oldPos.offset;
    newPos.mathPath = oldPos.mathPath;
    
    return newPos;
}

/**
 * @brief 将 NewCursorPosition 转换为 UnifiedCursorPosition
 * @param newPos 新的光标位置
 * @return 光标位置
 */
UnifiedCursorPosition CursorPositionAdapter::toOld(const NewCursorPosition& newPos) {
    UnifiedCursorPosition oldPos;
    
    // 直接复制字段
    oldPos.blockIndex = newPos.blockIndex;
    oldPos.offset = newPos.offset;
    oldPos.mathPath = newPos.mathPath;
    
    return oldPos;
}

} // namespace QtWordEditor
