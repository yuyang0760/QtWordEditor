
/**
 * @file CursorPositionAdapter.cpp
 * @brief 光标位置适配器实现
 * 
 * 在各种光标位置结构体之间进行转换
 * - CursorPosition <-> UnifiedCursorPosition
 */

#include "editcontrol/cursor/CursorPositionAdapter.h"

namespace QtWordEditor {

/**
 * @brief 将 CursorPosition 转换为 UnifiedCursorPosition
 * @param cursorPos 旧格式的光标位置
 * @return 新格式的光标位置
 */
UnifiedCursorPosition CursorPositionAdapter::cursorToUnified(const CursorPosition &cursorPos) {
    UnifiedCursorPosition unifiedPos;
    
    // 复制基本字段
    unifiedPos.blockIndex = cursorPos.blockIndex;
    unifiedPos.offset = cursorPos.offset;
    
    // 注意：旧格式的 inMathSpan 等信息暂时不转换到 mathPath
    // 因为 mathPath 需要更复杂的坐标路径信息
    // 未来可以根据需要完善这部分
    
    return unifiedPos;
}

/**
 * @brief 将 UnifiedCursorPosition 转换为 CursorPosition
 * @param unifiedPos 新格式的光标位置
 * @return 旧格式的光标位置
 */
CursorPosition CursorPositionAdapter::unifiedToCursor(const UnifiedCursorPosition &unifiedPos) {
    CursorPosition cursorPos;
    
    // 复制基本字段
    cursorPos.blockIndex = unifiedPos.blockIndex;
    cursorPos.offset = unifiedPos.offset;
    
    // 检查是否在公式模式
    if (unifiedPos.isMathMode()) {
        cursorPos.inMathSpan = true;
        // 注意：mathSpan、mathChildIndex、mathChildOffset 需要根据 mathPath 计算
        // 未来可以根据需要完善这部分
    } else {
        cursorPos.inMathSpan = false;
    }
    
    return cursorPos;
}

} // namespace QtWordEditor

