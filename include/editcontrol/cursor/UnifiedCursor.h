#ifndef UNIFIEDCURSOR_H
#define UNIFIEDCURSOR_H

#include <QObject>
#include <QPointF>
#include "core/Global.h"
#include "core/document/CharacterStyle.h"
#include "editcontrol/cursor/Cursor.h"

namespace QtWordEditor {

class Document;
class InlineSpan;
class MathItem;
class RowContainerItem;
class NumberItem;

/**
 * @brief 统一光标模式枚举
 * 
 * 定义光标可以处于的不同模式
 */
enum class CursorMode {
    DocumentMode,       // 普通文档模式
    MathContainerMode,  // 公式容器模式
    MathNumberMode      // 公式数字模式
};

/**
 * @brief 统一光标位置结构体
 * 
 * 整合了文档光标和公式光标的位置信息
 */
struct UnifiedCursorPosition {
    // ========== 文档模式字段 ==========
    int blockIndex = -1;              ///< 块索引
    int offset = 0;                    ///< 块内偏移量
    
    // ========== 公式模式字段 ==========
    bool inMathSpan = false;           ///< 是否在公式内部
    InlineSpan *mathSpan = nullptr;    ///< 当前所在的公式元素
    MathItem *mathItem = nullptr;     ///< 当前所在的 MathItem
    RowContainerItem *mathContainer = nullptr;  ///< 公式容器（容器模式）
    NumberItem *mathNumberItem = nullptr;       ///< 数字项（数字模式）
    int mathChildIndex = -1;          ///< 在公式容器中的子元素索引
    int mathChildOffset = 0;           ///< 在公式子元素内部的偏移量
    
    // ========== 当前模式 ==========
    CursorMode mode = CursorMode::DocumentMode;  ///< 当前光标模式
    
    /**
     * @brief 相等性比较运算符
     * @param other 要比较的另一个光标位置
     * @return 如果两个位置相等返回true
     */
    bool operator==(const UnifiedCursorPosition &other) const {
        return blockIndex == other.blockIndex 
               && offset == other.offset
               && inMathSpan == other.inMathSpan
               && mathSpan == other.mathSpan
               && mathItem == other.mathItem
               && mathContainer == other.mathContainer
               && mathNumberItem == other.mathNumberItem
               && mathChildIndex == other.mathChildIndex
               && mathChildOffset == other.mathChildOffset
               && mode == other.mode;
    }
    
    /**
     * @brief 不等性比较运算符
     * @param other 要比较的另一个光标位置
     * @return 如果两个位置不等返回true
     */
    bool operator!=(const UnifiedCursorPosition &other) const {
        return !(*this == other);
    }
};

/**
 * @brief 统一光标类
 * 
 * 整合了普通文档光标和数学公式光标的功能
 * 提供统一的光标管理接口
 */
class UnifiedCursor : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief 构造函数
     * @param document 关联的文档
     * @param parent 父对象指针，默认为nullptr
     */
    explicit UnifiedCursor(Document *document, QObject *parent = nullptr);
    
    /**
     * @brief 析构函数
     */
    ~UnifiedCursor() override;
    
    // ========== 模式切换 ==========
    
    /**
     * @brief 设置光标模式
     * @param mode 新的光标模式
     */
    void setMode(CursorMode mode);
    
    /**
     * @brief 获取当前光标模式
     * @return 当前光标模式
     */
    CursorMode mode() const;
    
    // ========== 位置管理 ==========
    
    /**
     * @brief 获取当前光标位置
     * @return 当前光标位置结构体
     */
    UnifiedCursorPosition position() const;
    
    /**
     * @brief 设置光标位置
     * @param pos 光标位置结构体
     */
    void setPosition(const UnifiedCursorPosition &pos);
    
    // ========== 文档模式方法 ==========
    
    /**
     * @brief 设置文档模式光标位置
     * @param blockIndex 块索引
     * @param offset 块内偏移量
     */
    void setDocumentPosition(int blockIndex, int offset);
    
    /**
     * @brief 向左移动光标
     */
    void moveLeft();
    
    /**
     * @brief 向右移动光标
     */
    void moveRight();
    
    /**
     * @brief 向上移动光标
     */
    void moveUp();
    
    /**
     * @brief 向下移动光标
     */
    void moveDown();
    
    /**
     * @brief 移动到行首
     */
    void moveToStartOfLine();
    
    /**
     * @brief 移动到行尾
     */
    void moveToEndOfLine();
    
    /**
     * @brief 移动到文档开头
     */
    void moveToStartOfDocument();
    
    /**
     * @brief 移动到文档结尾
     */
    void moveToEndOfDocument();
    
    /**
     * @brief 在光标位置插入文本
     * @param text 要插入的文本
     * @param style 文本的字符样式
     */
    void insertText(const QString &text, const CharacterStyle &style);
    
    /**
     * @brief 删除光标前一个字符
     */
    void deletePreviousChar();
    
    /**
     * @brief 删除光标后一个字符
     */
    void deleteNextChar();
    
    // ========== 公式模式方法 ==========
    
    /**
     * @brief 设置公式容器模式光标位置
     * @param container 光标所在的行容器
     * @param position 光标在容器中的位置索引
     */
    void setMathContainerPosition(RowContainerItem *container, int position);
    
    /**
     * @brief 设置公式数字模式光标位置
     * @param numberItem 光标所在的数字项
     * @param position 光标在数字中的字符偏移
     */
    void setMathNumberPosition(NumberItem *numberItem, int position);
    
    /**
     * @brief 在公式中向左移动光标
     */
    void mathMoveLeft();
    
    /**
     * @brief 在公式中向右移动光标
     */
    void mathMoveRight();
    
    /**
     * @brief 在公式中向上移动光标
     */
    void mathMoveUp();
    
    /**
     * @brief 在公式中向下移动光标
     */
    void mathMoveDown();
    
    /**
     * @brief 移动到公式父容器
     */
    void mathMoveToParent();
    
    // ========== 退出公式模式 ==========
    
    /**
     * @brief 退出公式模式，回到文档模式
     */
    void exitMathMode();
    
signals:
    /**
     * @brief 光标位置发生变化时发出的信号
     * @param pos 新的光标位置
     */
    void positionChanged(const UnifiedCursorPosition &pos);
    
    /**
     * @brief 光标模式发生变化时发出的信号
     * @param newMode 新的光标模式
     * @param oldMode 旧的光标模式
     */
    void modeChanged(CursorMode newMode, CursorMode oldMode);
    
private:
    Document *m_document;       ///< 关联的文档
    UnifiedCursorPosition m_position;  ///< 当前光标位置
};

} // namespace QtWordEditor

Q_DECLARE_METATYPE(QtWordEditor::UnifiedCursorPosition)

#endif // UNIFIEDCURSOR_H
