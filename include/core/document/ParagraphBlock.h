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
 * @brief The ParagraphBlock class represents a text paragraph.
 *
 * It contains a list of inline spans and paragraph-level formatting.
 */
class ParagraphBlock : public Block
{
    Q_OBJECT
public:
    explicit ParagraphBlock(QObject *parent = nullptr);
    ParagraphBlock(const ParagraphBlock &other);
    ~ParagraphBlock() override;

    // ========== 文本操作 ==========
    QString text() const;
    void setText(const QString &text);
    void insert(int position, const QString &text, const CharacterStyle &style);
    void remove(int position, int length);

    // ========== InlineSpan 访问 ==========
    int inlineSpanCount() const;
    InlineSpan *inlineSpan(int index) const;
    void addInlineSpan(InlineSpan *span);
    void insertInlineSpan(int index, InlineSpan *span);
    void insertInlineSpanAtPosition(int position, InlineSpan *span);
    void removeInlineSpan(InlineSpan *span);
    void removeInlineSpanAt(int index);
    void clearInlineSpans();

    // ========== 段落样式 ==========
    ParagraphStyle paragraphStyle() const;
    void setParagraphStyle(const ParagraphStyle &style);

    // ========== 重写 Block 方法 ==========
    int length() const override;
    bool isEmpty() const override;
    Block *clone() const override;
    
    // ========== 辅助方法 ==========
    
    // 查找 InlineSpan 索引
    int findInlineSpanIndex(int globalPosition, int *positionInSpan = nullptr) const;
    
    // 获取指定位置的字符样式
    CharacterStyle styleAt(int position) const;
    
    // 设置指定范围的字符样式
    void setStyle(int start, int length, const CharacterStyle &style);
    
    // 检查范围是否跨多个 Span
    bool isRangeSpansMultipleSpans(int start, int end) const;
    
    // 获取指定位置的字符
    QChar characterAt(int position) const;

signals:
    void textChanged();
    void inlineSpansChanged();

private:
    // 辅助：保持 Span 一致性
    void mergeAdjacentSpans();
    
    // 辅助：验证位置和长度参数
    bool validatePositionAndLength(int& position, int& length) const;

private:
    QList<InlineSpan*> m_inlineSpans;  // 统一管理文本和公式
    ParagraphStyle m_paragraphStyle;
};

} // namespace QtWordEditor

#endif // PARAGRAPHBLOCK_H
