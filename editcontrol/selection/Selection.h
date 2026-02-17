#ifndef SELECTION_H
#define SELECTION_H

#include <QObject>
#include <QList>
#include <QRectF>
#include "core/Global.h"

namespace QtWordEditor {

struct SelectionRange
{
    int startBlock = -1;
    int startOffset = 0;
    int endBlock = -1;
    int endOffset = 0;

    bool isEmpty() const {
        return startBlock == endBlock && startOffset == endOffset;
    }

    void normalize() {
        if (startBlock > endBlock || (startBlock == endBlock && startOffset > endOffset)) {
            qSwap(startBlock, endBlock);
            qSwap(startOffset, endOffset);
        }
    }
};

class Document;

/**
 * @brief The Selection class manages text selections, supporting multiple ranges.
 */
class Selection : public QObject
{
    Q_OBJECT
public:
    explicit Selection(Document *document, QObject *parent = nullptr);
    ~Selection() override;

    Document *document() const;

    // Single‑range selection
    void setRange(const SelectionRange &range);
    void setRange(int block1, int offset1, int block2, int offset2);
    SelectionRange range() const;

    // Multi‑range selection
    QList<SelectionRange> ranges() const;
    void addRange(const SelectionRange &range);
    void clear();

    // Extend selection (like Shift+click)
    void extend(int block, int offset);

    // Query
    bool isEmpty() const;
    QString selectedText() const;

signals:
    void selectionChanged();

private:
    Document *m_document;
    QList<SelectionRange> m_ranges;
};

} // namespace QtWordEditor

#endif // SELECTION_H