#ifndef PATHSEGMENT_H
#define PATHSEGMENT_H

#include "graphics/formula/MathItem.h"

namespace QtWordEditor {

/**
 * @brief 坐标路径段
 * 
 * 描述路径中的一个步骤，包含：
 * - container: 当前所在的容器
 * - childIndex: 在容器中的子元素索引
 * - childOffset: 在子元素内部的偏移量
 */
struct PathSegment {
    MathItem* container = nullptr;      ///< 当前容器
    int childIndex = -1;                  ///< 子元素索引
    int childOffset = 0;                  ///< 子元素内偏移
    
    /**
     * @brief 默认构造函数
     */
    PathSegment() = default;
    
    /**
     * @brief 带参数的构造函数
     * @param container 容器指针
     * @param childIndex 子元素索引
     * @param childOffset 子元素内偏移
     */
    PathSegment(MathItem* container, int childIndex, int childOffset)
        : container(container), childIndex(childIndex), childOffset(childOffset) {}
    
    /**
     * @brief 检查路径段是否有效
     * @return 如果有效返回 true
     */
    bool isValid() const {
        return container != nullptr && childIndex >= 0;
    }
    
    /**
     * @brief 相等性比较
     * @param other 要比较的另一个路径段
     * @return 如果相等返回 true
     */
    bool operator==(const PathSegment& other) const {
        return container == other.container
            && childIndex == other.childIndex
            && childOffset == other.childOffset;
    }
    
    /**
     * @brief 不等性比较
     * @param other 要比较的另一个路径段
     * @return 如果不等返回 true
     */
    bool operator!=(const PathSegment& other) const {
        return !(*this == other);
    }
};

} // namespace QtWordEditor

#endif // PATHSEGMENT_H
