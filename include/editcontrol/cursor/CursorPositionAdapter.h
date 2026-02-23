#ifndef CURSORPOSITIONADAPTER_H
#define CURSORPOSITIONADAPTER_H

#include "editcontrol/cursor/UnifiedCursor.h"
#include "editcontrol/cursor/NewCursorPosition.h"

namespace QtWordEditor {

/**
 * @brief 光标位置适配器
 * 
 * 在旧的 UnifiedCursorPosition 和新的 NewCursorPosition 之间进行转换
 */
class CursorPositionAdapter {
public:
    /**
     * @brief 将旧的 UnifiedCursorPosition 转换为新的 NewCursorPosition
     * @param oldPos 旧的光标位置
     * @return 新的光标位置
     */
    static NewCursorPosition toNew(const UnifiedCursorPosition& oldPos);
    
    /**
     * @brief 将新的 NewCursorPosition 转换为旧的 UnifiedCursorPosition
     * @param newPos 新的光标位置
     * @return 旧的光标位置
     */
    static UnifiedCursorPosition toOld(const NewCursorPosition& newPos);
    
private:
    /**
     * @brief 从 RowContainerItem 构建坐标路径
     * @param container 容器指针
     * @param childIndex 子元素索引
     * @param childOffset 子元素内偏移
     * @param path 要填充的坐标路径
     */
    static void buildPathFromContainer(
        RowContainerItem* container,
        int childIndex,
        int childOffset,
        CoordinatePath& path
    );
    
    /**
     * @brief 从 NumberItem 构建坐标路径
     * @param numberItem 数字项指针
     * @param offset 偏移量
     * @param path 要填充的坐标路径
     */
    static void buildPathFromNumber(
        NumberItem* numberItem,
        int offset,
        CoordinatePath& path
    );
};

} // namespace QtWordEditor

#endif // CURSORPOSITIONADAPTER_H
