#ifndef CURSORPOSITION_H
#define CURSORPOSITION_H

#include "core/Global.h"

namespace QtWordEditor {

/**
 * @brief 光标位置结构体
 */
struct CursorPosition {
    int blockIndex = 0;        ///< 块索引
    int offset = 0;             ///< 块内偏移量
    bool inMathSpan = false;    ///< 是否在公式块内（旧字段，兼容）

    /**
     * @brief 默认构造函数
     */
    CursorPosition() = default;

    /**
     * @brief 带参数的构造函数
     * @param block 块索引
     * @param off 块内偏移量
     */
    CursorPosition(int block, int off) : blockIndex(block), offset(off), inMathSpan(false) {}

    /**
     * @brief 相等比较运算符
     * @param other 另一个光标位置
     * @return 是否相等
     */
    bool operator==(const CursorPosition &other) const {
        return blockIndex == other.blockIndex && offset == other.offset;
    }

    /**
     * @brief 不等比较运算符
     * @param other 另一个光标位置
     * @return 是否不等
     */
    bool operator!=(const CursorPosition &other) const {
        return !(*this == other);
    }
};

} // namespace QtWordEditor

Q_DECLARE_METATYPE(QtWordEditor::CursorPosition)

#endif // CURSORPOSITION_H
