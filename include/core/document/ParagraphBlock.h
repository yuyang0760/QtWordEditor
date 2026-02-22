#ifndef PARAGRAPHBLOCK_H
#define PARAGRAPHBLOCK_H

#include "Block.h"
#include "ParagraphStyle.h"
#include "TextSpan.h"
#include "InlineSpan.h"
#include <QList>
#include "core/Global.h"

namespace QtWordEditor {

/**
 * @brief The ParagraphBlock class represents a text paragraph.
 *
 * It contains a list of spans and paragraph-level formatting.
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

    // ========== InlineSpan管理 ==========
    int inlineSpanCount() const;
    InlineSpan *inlineSpan(int index) const;
    void addInlineSpan(InlineSpan *span);
    void insertInlineSpan(int index, InlineSpan *span);
    void removeInlineSpan(int index);
    void clearInlineSpans();

    // ========== 段落样式 ==========
    ParagraphStyle paragraphStyle() const;
    void setParagraphStyle(const ParagraphStyle &style);

    // ========== Block接口重写 ==========
    int length() const override;
    bool isEmpty() const override;
    Block *clone() const override;
    
    // ========== 辅助方法 ==========
    int findInlineSpanIndex(int globalPosition, int *positionInSpan = nullptr) const;
    CharacterStyle styleAt(int position) const;
    void setStyle(int start, int length, const CharacterStyle &style);
    QChar characterAt(int position) const;

signals:
    void textChanged();

private:
    void mergeAdjacentTextSpans();
    bool validatePositionAndLength(int& position, int& length) const;

private:
    QList<InlineSpan*> m_inlineSpans;
    ParagraphStyle m_paragraphStyle;
};

} // namespace QtWordEditor

#endif // PARAGRAPHBLOCK_H