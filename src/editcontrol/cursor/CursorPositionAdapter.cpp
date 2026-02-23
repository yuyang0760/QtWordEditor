/**
 * @file CursorPositionAdapter.cpp
 * @brief 光标位置适配器实现
 * 
 * 在旧的 UnifiedCursorPosition 和新的 NewCursorPosition 之间进行转换
 */

#include "editcontrol/cursor/CursorPositionAdapter.h"
#include "graphics/formula/RowContainerItem.h"
#include "graphics/formula/NumberItem.h"

namespace QtWordEditor {

/**
 * @brief 将旧的 UnifiedCursorPosition 转换为新的 NewCursorPosition
 * @param oldPos 旧的光标位置
 * @return 新的光标位置
 */
NewCursorPosition CursorPositionAdapter::toNew(const UnifiedCursorPosition& oldPos) {
    NewCursorPosition newPos;
    
    // 复制文档位置
    newPos.blockIndex = oldPos.blockIndex;
    newPos.offset = oldPos.offset;
    
    // 如果在公式模式，构建坐标路径
    if (oldPos.inMathSpan) {
        CoordinatePath path;
        
        switch (oldPos.mode) {
            case CursorMode::DocumentMode:
                // 文档模式，不需要路径
                break;
                
            case CursorMode::MathContainerMode:
                // 容器模式
                if (oldPos.mathContainer != nullptr) {
                    buildPathFromContainer(
                        oldPos.mathContainer,
                        oldPos.mathChildIndex,
                        oldPos.mathChildOffset,
                        path
                    );
                }
                break;
                
            case CursorMode::MathNumberMode:
                // 数字模式
                if (oldPos.mathNumberItem != nullptr) {
                    buildPathFromNumber(
                        oldPos.mathNumberItem,
                        oldPos.mathChildOffset,
                        path
                    );
                }
                break;
        }
        
        // 如果路径不为空，设置到新位置
        if (!path.isEmpty()) {
            newPos.mathPath = std::move(path);
        }
    }
    
    return newPos;
}

/**
 * @brief 将新的 NewCursorPosition 转换为旧的 UnifiedCursorPosition
 * @param newPos 新的光标位置
 * @return 旧的光标位置
 */
UnifiedCursorPosition CursorPositionAdapter::toOld(const NewCursorPosition& newPos) {
    UnifiedCursorPosition oldPos;
    
    // 复制文档位置
    oldPos.blockIndex = newPos.blockIndex;
    oldPos.offset = newPos.offset;
    
    // 如果在公式模式，解析坐标路径
    if (newPos.isMathMode() && newPos.mathPath->isValid() && !newPos.mathPath->isEmpty()) {
        oldPos.inMathSpan = true;
        
        const CoordinatePath& path = *newPos.mathPath;
        const PathSegment& lastSegment = path.top();
        
        // 设置基础信息
        oldPos.mathItem = lastSegment.container;
        
        // 根据容器类型判断模式
        if (dynamic_cast<NumberItem*>(lastSegment.container) != nullptr) {
            // 数字模式
            oldPos.mode = CursorMode::MathNumberMode;
            oldPos.mathNumberItem = dynamic_cast<NumberItem*>(lastSegment.container);
            oldPos.mathChildOffset = lastSegment.childOffset;
        } else if (dynamic_cast<RowContainerItem*>(lastSegment.container) != nullptr) {
            // 容器模式
            oldPos.mode = CursorMode::MathContainerMode;
            oldPos.mathContainer = dynamic_cast<RowContainerItem*>(lastSegment.container);
            oldPos.mathChildIndex = lastSegment.childIndex;
            oldPos.mathChildOffset = lastSegment.childOffset;
        } else {
            // 默认容器模式
            oldPos.mode = CursorMode::MathContainerMode;
            oldPos.mathContainer = dynamic_cast<RowContainerItem*>(lastSegment.container);
            oldPos.mathChildIndex = lastSegment.childIndex;
            oldPos.mathChildOffset = lastSegment.childOffset;
        }
    } else {
        // 文档模式
        oldPos.mode = CursorMode::DocumentMode;
        oldPos.inMathSpan = false;
    }
    
    return oldPos;
}

/**
 * @brief 从 RowContainerItem 构建坐标路径
 * @param container 容器指针
 * @param childIndex 子元素索引
 * @param childOffset 子元素内偏移
 * @param path 要填充的坐标路径
 */
void CursorPositionAdapter::buildPathFromContainer(
    RowContainerItem* container,
    int childIndex,
    int childOffset,
    CoordinatePath& path
) {
    if (container == nullptr) {
        return;
    }
    
    // 从当前容器开始向上遍历，构建完整路径
    MathItem* current = container;
    std::vector<PathSegment> tempSegments;
    
    while (current != nullptr) {
        MathItem* parent = current->parentMathItem();
        
        if (parent != nullptr) {
            // 找到当前项在父容器中的索引
            int index = parent->indexOfChild(current);
            if (index >= 0) {
                tempSegments.push_back(PathSegment(parent, index, 0));
            }
        }
        
        current = parent;
    }
    
    // 反转路径，从根到叶子
    for (auto it = tempSegments.rbegin(); it != tempSegments.rend(); ++it) {
        path.push(*it);
    }
    
    // 添加最后一段（当前容器）
    path.push(PathSegment(container, childIndex, childOffset));
}

/**
 * @brief 从 NumberItem 构建坐标路径
 * @param numberItem 数字项指针
 * @param offset 偏移量
 * @param path 要填充的坐标路径
 */
void CursorPositionAdapter::buildPathFromNumber(
    NumberItem* numberItem,
    int offset,
    CoordinatePath& path
) {
    if (numberItem == nullptr) {
        return;
    }
    
    // 先从父容器开始构建路径
    MathItem* parent = numberItem->parentMathItem();
    if (parent != nullptr && dynamic_cast<RowContainerItem*>(parent) != nullptr) {
        RowContainerItem* parentContainer = dynamic_cast<RowContainerItem*>(parent);
        int index = parent->indexOfChild(numberItem);
        
        if (index >= 0) {
            buildPathFromContainer(parentContainer, index, 0, path);
            
            // 修改最后一段的偏移量
            if (!path.isEmpty()) {
                path.top().childOffset = offset;
            }
        }
    } else {
        // 没有父容器，直接添加数字项
        path.push(PathSegment(numberItem, 0, offset));
    }
}

} // namespace QtWordEditor
