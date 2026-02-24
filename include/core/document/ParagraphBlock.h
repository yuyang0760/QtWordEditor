/**
 * @file ParagraphBlock.h
 * @brief 段落块类
 * 
 * 表示一个文本段落，包含内联元素列表和段落级格式化信息。
 * 支持文本操作、内联元素管理和段落样式设置。
 */

#ifndef PARAGRAPHBLOCK_H
#define PARAGRAPHBLOCK_H

#include "Block.h"
#include "ParagraphStyle.h"
#include "InlineSpan.h"
#include "TextSpan.h"
#include <QList>
#include "core/Global.h"

namespace QtWordEditor {

/**
 * @brief 段落块类
 * 
 * 表示一个文本段落，包含内联元素列表（文本和公式）和段落级格式化信息。
 * 是文档中最常用的块类型，支持丰富的文本编辑操作。
 */
class ParagraphBlock : public Block
{
    Q_OBJECT
public:
    /**
     * @brief 构造函数
     * @param parent 父对象指针，默认为nullptr
     */
    explicit ParagraphBlock(QObject *parent = nullptr);
    
    /**
     * @brief 拷贝构造函数
     * @param other 要拷贝的段落块
     */
    ParagraphBlock(const ParagraphBlock &other);
    
    /**
     * @brief 析构函数
     */
    ~ParagraphBlock() override;

    // ========== 文本操作 ==========
    
    /**
     * @brief 获取段落的完整文本
     * @return 段落文本字符串
     */
    QString text() const;
    
    /**
     * @brief 设置段落的完整文本
     * @param text 新的文本内容
     */
    void setText(const QString &text);
    
    /**
     * @brief 在指定位置插入文本
     * @param position 插入位置
     * @param text 要插入的文本
     * @param style 字符样式
     */
    void insert(int position, const QString &text, const CharacterStyle &style);
    
    /**
     * @brief 移除指定位置和长度的文本
     * @param position 起始位置
     * @param length 要移除的长度
     */
    void remove(int position, int length);

    // ========== InlineSpan 访问 ==========
    
    /**
     * @brief 获取内联元素数量
     * @return 内联元素总数
     */
    int inlineSpanCount() const;
    
    /**
     * @brief 获取指定索引的内联元素
     * @param index 索引位置
     * @return 内联元素指针
     */
    InlineSpan *inlineSpan(int index) const;
    
    /**
     * @brief 添加内联元素到末尾
     * @param span 要添加的内联元素
     */
    void addInlineSpan(InlineSpan *span);
    
    /**
     * @brief 在指定索引插入内联元素
     * @param index 插入位置
     * @param span 要插入的内联元素
     */
    void insertInlineSpan(int index, InlineSpan *span);
    
    /**
     * @brief 在指定文本位置插入内联元素
     * @param position 文本位置
     * @param span 要插入的内联元素
     */
    void insertInlineSpanAtPosition(int position, InlineSpan *span);
    
    /**
     * @brief 移除指定的内联元素
     * @param span 要移除的内联元素
     */
    void removeInlineSpan(InlineSpan *span);
    
    /**
     * @brief 移除指定索引的内联元素
     * @param index 要移除的索引
     */
    void removeInlineSpanAt(int index);
    
    /**
     * @brief 清除所有内联元素
     */
    void clearInlineSpans();

    // ========== 段落样式 ==========
    
    /**
     * @brief 获取段落样式
     * @return 段落样式对象
     */
    ParagraphStyle paragraphStyle() const;
    
    /**
     * @brief 设置段落样式
     * @param style 新的段落样式
     */
    void setParagraphStyle(const ParagraphStyle &style);

    // ========== 重写 Block 方法 ==========
    
    /**
     * @brief 获取段落长度
     * @return 段落文本长度
     */
    int length() const override;
    
    /**
     * @brief 判断段落是否为空
     * @return 如果段落为空返回true
     */
    bool isEmpty() const override;
    
    /**
     * @brief 克隆段落
     * @return 克隆的段落指针
     */
    Block *clone() const override;
    
    // ========== 辅助方法 ==========
    
    /**
     * @brief 查找指定全局位置对应的内联元素索引
     * @param globalPosition 全局文本位置
     * @param positionInSpan 输出参数，返回在找到的内联元素内的位置
     * @return 内联元素索引
     */
    int findInlineSpanIndex(int globalPosition, int *positionInSpan = nullptr) const;
    
    /**
     * @brief 获取指定位置的字符样式
     * @param position 文本位置
     * @return 字符样式对象
     */
    CharacterStyle styleAt(int position) const;
    
    /**
     * @brief 设置指定范围的字符样式
     * @param start 起始位置
     * @param length 范围长度
     * @param style 字符样式
     */
    void setStyle(int start, int length, const CharacterStyle &style);
    
    /**
     * @brief 检查范围是否跨多个内联元素
     * @param start 起始位置
     * @param end 结束位置
     * @return 如果跨多个内联元素返回true
     */
    bool isRangeSpansMultipleSpans(int start, int end) const;
    
    /**
     * @brief 获取指定位置的字符
     * @param position 文本位置
     * @return 字符值
     */
    QChar characterAt(int position) const;

signals:
    /** @brief 文本内容发生变化时发出的信号 */
    void textChanged();
    
    /** @brief 内联元素列表发生变化时发出的信号 */
    void inlineSpansChanged();

private:
    /**
     * @brief 合并相邻且样式相同的文本内联元素
     * 保持内联元素列表的一致性和简洁性
     */
    void mergeAdjacentSpans();
    
    /**
     * @brief 验证位置和长度参数
     * @param position 起始位置
     * @param length 长度
     * @return 如果参数有效返回true
     */
    bool validatePositionAndLength(int& position, int& length) const;

private:
    QList<InlineSpan*> m_inlineSpans;  ///< 统一管理文本和公式的内联元素列表
    ParagraphStyle m_paragraphStyle;   ///< 段落样式
};

} // namespace QtWordEditor

#endif // PARAGRAPHBLOCK_H
