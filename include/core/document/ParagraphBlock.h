#ifndef PARAGRAPHBLOCK_H
#define PARAGRAPHBLOCK_H

#include "Block.h"
#include "ParagraphStyle.h"
#include "Span.h"
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

    // Text operations
    QString text() const;
    void setText(const QString &text);
    void insert(int position, const QString &text, const CharacterStyle &style);
    void remove(int position, int length);

    // Span access
    int spanCount() const;
    Span span(int index) const;
    void addSpan(const Span &span);
    void setSpan(int index, const Span &span);

    // Paragraph style
    ParagraphStyle paragraphStyle() const;
    void setParagraphStyle(const ParagraphStyle &style);

    // Overrides from Block
    int length() const override;
    bool isEmpty() const override;
    Block *clone() const override;

signals:
    void textChanged();

private:
    // Helper to maintain span consistency after modification
    void mergeAdjacentSpans();

private:
    QList<Span> m_spans;
    ParagraphStyle m_paragraphStyle;
};

} // namespace QtWordEditor

#endif // PARAGRAPHBLOCK_H