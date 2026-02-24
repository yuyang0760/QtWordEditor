#ifndef UNIFIEDCURSOR_H
#define UNIFIEDCURSOR_H

#include <QObject>
#include <QPointF>
#include <optional>
#include "core/Global.h"
#include "core/document/CharacterStyle.h"
#include "editcontrol/cursor/CursorPosition.h"
#include "editcontrol/cursor/CoordinatePath.h"

namespace QtWordEditor {

class Document;
class InlineSpan;
class MathItem;

/**
 * @brief 统一光标位置结构体（无模式版本）
 * 
 * 整合了文档光标和公式光标的位置信息
 * 完全采用坐标路径系统，不再区分模式
 */
struct UnifiedCursorPosition {
    int blockIndex = -1;              ///< 块索引
    int offset = 0;                    ///< 块内偏移量
    std::optional<CoordinatePath> mathPath;  ///< 公式坐标路径（可选，存在表示在公式中）
    int mathTextOffset = 0;            ///< 公式内的文本偏移（仅在 mathPath 存在时使用）
    
    /**
     * @brief 相等性比较运算符
     * @param other 要比较的另一个光标位置
     * @return 如果两个位置相等返回true
     */
    bool operator==(const UnifiedCursorPosition &other) const {
        return blockIndex == other.blockIndex 
               && offset == other.offset
               && mathPath == other.mathPath
               && mathTextOffset == other.mathTextOffset;
    }
    
    /**
     * @brief 不等性比较运算符
     * @param other 要比较的另一个光标位置
     * @return 如果两个位置不等返回true
     */
    bool operator!=(const UnifiedCursorPosition &other) const {
        return !(*this == other);
    }
    
    /**
     * @brief 判断是否在公式模式
     * @return 如果在公式中返回true
     */
    bool isMathMode() const {
        return mathPath.has_value() && mathPath->isValid();
    }
    
    /**
     * @brief 判断是否在文档模式
     * @return 如果在文档中返回true
     */
    bool isDocumentMode() const {
        return !isMathMode();
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
    
    /**
     * @brief 获取关联的文档
     * @return 当前文档指针
     */
    Document *document() const;
    
    // ========== 位置管理（统一接口） ==========
    
    /**
     * @brief 获取当前光标位置
     * @return 当前光标位置结构体
     */
    UnifiedCursorPosition unifiedPosition() const;
    
    /**
     * @brief 设置光标位置
     * @param pos 光标位置结构体
     */
    void setUnifiedPosition(const UnifiedCursorPosition &pos);
    
    // ========== 文档位置方法 ==========
    
    /**
     * @brief 设置文档光标位置
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
    
    // ========== 公式位置方法 ==========
    
    /**
     * @brief 设置公式光标位置
     * @param mathPath 公式坐标路径
     */
    void setMathPosition(const CoordinatePath &mathPath);
    
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
    
    // ========== 公式内文本编辑 ==========
    
    /**
     * @brief 在公式内光标位置插入文本
     * @param text 要插入的文本
     * @param style 文本的字符样式
     */
    void mathInsertText(const QString &text, const CharacterStyle &style);
    
    /**
     * @brief 删除公式内光标前一个字符
     */
    void mathDeletePreviousChar();
    
    /**
     * @brief 删除公式内光标后一个字符
     */
    void mathDeleteNextChar();
    
    /**
     * @brief 获取统一光标位置的字符样式
     * @return 当前位置的字符样式
     */
    CharacterStyle styleAtUnifiedPosition() const;
    
signals:
    /**
     * @brief 光标位置发生变化时发出的信号
     * @param pos 新的光标位置
     */
    void unifiedPositionChanged(const UnifiedCursorPosition &pos);
    
private:
    Document *m_document;       ///< 关联的文档
    UnifiedCursorPosition m_position;  ///< 当前光标位置（使用坐标路径系统）
    
    /**
     * @brief 在内部位置变化时，发出位置变化信号
     */
    void emitPositionChangedSignals();
};

} // namespace QtWordEditor

Q_DECLARE_METATYPE(QtWordEditor::UnifiedCursorPosition)

#endif // UNIFIEDCURSOR_H
