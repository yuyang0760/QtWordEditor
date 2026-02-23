#ifndef NEWCURSORPOSITION_H
#define NEWCURSORPOSITION_H

#include <optional>
#include "editcontrol/cursor/CoordinatePath.h"

namespace QtWordEditor {

/**
 * @brief 新的光标位置（使用坐标路径）
 * 
 * 整合了文档位置和公式坐标路径
 */
struct NewCursorPosition {
    // ========== 文档位置 ==========
    int blockIndex = -1;              ///< 块索引
    int offset = 0;                    ///< 块内偏移量
    
    // ========== 公式坐标路径（可选）==========
    std::optional<CoordinatePath> mathPath;  ///< 公式坐标路径
    
    // ========== 默认构造函数 ==========
    NewCursorPosition() = default;
    
    // ========== 文档位置构造函数 ==========
    /**
     * @brief 文档位置构造函数
     * @param blockIndex 块索引
     * @param offset 块内偏移量
     */
    NewCursorPosition(int blockIndex, int offset)
        : blockIndex(blockIndex), offset(offset) {}
    
    // ========== 公式位置构造函数 ==========
    /**
     * @brief 公式位置构造函数
     * @param blockIndex 块索引
     * @param offset 块内偏移量
     * @param path 公式坐标路径
     */
    NewCursorPosition(int blockIndex, int offset, CoordinatePath path)
        : blockIndex(blockIndex), offset(offset), mathPath(std::move(path)) {}
    
    // ========== 便捷查询 ==========
    
    /**
     * @brief 是否在文档模式
     * @return 如果在文档模式返回 true
     */
    bool isDocumentMode() const {
        return !mathPath.has_value();
    }
    
    /**
     * @brief 是否在公式模式
     * @return 如果在公式模式返回 true
     */
    bool isMathMode() const {
        return mathPath.has_value();
    }
    
    /**
     * @brief 检查位置是否有效
     * @return 如果有效返回 true
     */
    bool isValid() const {
        if (blockIndex < 0) {
            return false;
        }
        if (mathPath.has_value()) {
            return mathPath->isValid();
        }
        return true;
    }
    
    // ========== 比较操作 ==========
    
    /**
     * @brief 相等性比较
     * @param other 要比较的另一个位置
     * @return 如果相等返回 true
     */
    bool operator==(const NewCursorPosition& other) const {
        return blockIndex == other.blockIndex
            && offset == other.offset
            && mathPath == other.mathPath;
    }
    
    /**
     * @brief 不等性比较
     * @param other 要比较的另一个位置
     * @return 如果不等返回 true
     */
    bool operator!=(const NewCursorPosition& other) const {
        return !(*this == other);
    }
};

} // namespace QtWordEditor

#endif // NEWCURSORPOSITION_H
