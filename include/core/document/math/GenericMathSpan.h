/**
 * @file GenericMathSpan.h
 * @brief 通用公式容器数据类（简化版）
 * 
 * 使用 InlineSpan 列表，类似 ParagraphBlock 的设计，
 * 但没有段落、缩进、自动换行等复杂功能。
 * 专门用于公式内部的文本和公式混排。
 */

#ifndef GENERICMATHSPAN_H
#define GENERICMATHSPAN_H

#include "core/document/MathSpan.h"
#include "core/document/InlineSpan.h"
#include "core/document/CharacterStyle.h"
#include <QList>
#include "core/Global.h"

namespace QtWordEditor {

/**
 * @brief 通用公式容器数据类（简化版）
 * 
 * 使用 InlineSpan 列表，类似 ParagraphBlock 的设计，
 * 但没有段落、缩进、自动换行等复杂功能。
 * 专门用于公式内部的文本和公式混排。
 */
class GenericMathSpan : public MathSpan
{
    Q_OBJECT
public:
    /**
     * @brief 构造函数
     */
    explicit GenericMathSpan(QObject *parent = nullptr);

    /**
     * @brief 析构函数
     */
    ~GenericMathSpan() override;

    // ========== MathSpan 接口实现 ==========

    /**
     * @brief 获取公式元素类型
     * @return 固定返回 MathSpan::Generic
     */
    MathType mathType() const override { return Generic; }

    /**
     * @brief 判断是否是容器类型
     * @return 固定返回 true
     */
    bool isContainer() const override { return true; }

    /**
     * @brief 克隆当前对象
     * @return 新的 GenericMathSpan 指针
     */
    InlineSpan *clone() const override;

    // ========== InlineSpan 管理（类似 ParagraphBlock） ==========

    /**
     * @brief 获取所有 InlineSpan 列表
     * @return InlineSpan 列表
     */
    QList<InlineSpan*> spans() const;

    /**
     * @brief 获取 InlineSpan 数量
     * @return InlineSpan 数量
     */
    int spanCount() const;

    /**
     * @brief 获取指定位置的 InlineSpan
     * @param index 位置索引
     * @return InlineSpan 指针
     */
    InlineSpan *spanAt(int index) const;

    /**
     * @brief 在指定位置插入 InlineSpan
     * @param index 插入位置
     * @param span 要插入的 InlineSpan
     */
    void insertSpan(int index, InlineSpan *span);

    /**
     * @brief 在末尾追加 InlineSpan
     * @param span 要追加的 InlineSpan
     */
    void appendSpan(InlineSpan *span);

    /**
     * @brief 移除指定位置的 InlineSpan
     * @param index 要移除的位置
     */
    void removeSpanAt(int index);

    /**
     * @brief 清空所有 InlineSpan
     */
    void clearSpans();

    // ========== 文本编辑方法（类似 ParagraphBlock） ==========

    /**
     * @brief 获取所有文本内容
     * @return 文本内容
     */
    QString text() const;

    /**
     * @brief 在指定位置插入文本
     * @param position 插入位置
     * @param text 要插入的文本
     * @param style 字符样式
     */
    void insert(int position, const QString &text, const CharacterStyle &style);

    /**
     * @brief 删除指定位置和长度的文本
     * @param position 起始位置
     * @param length 要删除的长度
     */
    void remove(int position, int length);

    /**
     * @brief 查找指定全局位置对应的 InlineSpan 索引
     * @param globalPosition 全局位置
     * @param positionInSpan 输出：在找到的 span 内的位置
     * @return span 索引
     */
    int findInlineSpanIndex(int globalPosition, int *positionInSpan = nullptr) const;

    /**
     * @brief 获取指定位置的字符样式
     * @param position 位置
     * @return 字符样式
     */
    CharacterStyle styleAt(int position) const;

    /**
     * @brief 获取总长度
     * @return 总长度
     */
    int length() const;

signals:
    /**
     * @brief InlineSpan 列表变化信号
     */
    void spansChanged();

    /**
     * @brief 内容变化信号
     */
    void contentChanged();

private:
    // ========== 辅助方法 ==========

    /**
     * @brief 合并相邻且样式相同的 TextSpan
     */
    void mergeAdjacentSpans();

    QList<InlineSpan*> m_spans;  ///< InlineSpan 列表
};

} // namespace QtWordEditor

#endif // GENERICMATHSPAN_H
