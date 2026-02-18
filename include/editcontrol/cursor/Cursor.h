#ifndef CURSOR_H
#define CURSOR_H

#include <QObject>
#include <QPointF>
#include "core/Global.h"
#include "core/document/CharacterStyle.h"

namespace QtWordEditor {

/**
 * @brief 光标位置结构体，表示文档中的光标位置
 *
 * 包含块索引和块内字符偏移量，用于精确定位光标在文档中的位置。
 */
struct CursorPosition
{
    int blockIndex = -1;    ///< 块索引
    int offset = 0;         ///< 块内的字符偏移量

    /**
     * @brief 相等性比较运算符
     * @param other 要比较的另一个光标位置
     * @return 如果两个位置相等返回true
     */
    bool operator==(const CursorPosition &other) const {
        return blockIndex == other.blockIndex && offset == other.offset;
    }
    
    /**
     * @brief 不等性比较运算符
     * @param other 要比较的另一个光标位置
     * @return 如果两个位置不等返回true
     */
    bool operator!=(const CursorPosition &other) const {
        return !(*this == other);
    }
};

class Document;

/**
 * @brief 光标类，管理文档中的插入点位置和相关操作
 *
 * 该类负责：
 * 1. 维护光标的当前位置
 * 2. 处理光标的移动操作
 * 3. 执行基本的编辑操作（创建相应的命令）
 * 4. 发送位置变化信号
 */
class Cursor : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief 构造函数
     * @param document 关联的文档
     * @param parent 父对象指针，默认为nullptr
     */
    explicit Cursor(Document *document, QObject *parent = nullptr);
    
    /**
     * @brief 析构函数
     */
    ~Cursor() override;

    /**
     * @brief 获取关联的文档
     * @return 当前文档指针
     */
    Document *document() const;

    /**
     * @brief 获取当前光标位置
     * @return 当前光标位置结构体
     */
    CursorPosition position() const;
    
    /**
     * @brief 设置光标位置
     * @param blockIndex 块索引
     * @param offset 块内偏移量
     */
    void setPosition(int blockIndex, int offset);
    
    /**
     * @brief 设置光标位置
     * @param pos 光标位置结构体
     */
    void setPosition(const CursorPosition &pos);

    // ========== 光标移动方法 ==========
    
    /**
     * @brief 向左移动光标
     * 移动到前一个字符位置
     */
    void moveLeft();
    
    /**
     * @brief 向右移动光标
     * 移动到后一个字符位置
     */
    void moveRight();
    
    /**
     * @brief 向上移动光标
     * 移动到上一行相同水平位置
     */
    void moveUp();
    
    /**
     * @brief 向下移动光标
     * 移动到下一行相同水平位置
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

    // ========== 编辑操作方法（会创建相应命令）==========
    
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

signals:
    /**
     * @brief 光标位置发生变化时发出的信号
     * @param pos 新的光标位置
     */
    void positionChanged(const CursorPosition &pos);

private:
    Document *m_document;       ///< 关联的文档
    CursorPosition m_position;  ///< 当前光标位置
};

} // namespace QtWordEditor

Q_DECLARE_METATYPE(QtWordEditor::CursorPosition)

#endif // CURSOR_H