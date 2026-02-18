#ifndef SELECTION_H
#define SELECTION_H

#include <QObject>
#include <QList>
#include <QRectF>
#include "core/Global.h"
#include "editcontrol/cursor/Cursor.h"

namespace QtWordEditor {

struct SelectionRange
{
    int anchorBlock = -1;      ///< 锚点块索引（选择开始位置）
    int anchorOffset = 0;       ///< 锚点偏移（选择开始位置）
    int focusBlock = -1;       ///< 焦点块索引（选择结束位置）
    int focusOffset = 0;        ///< 焦点偏移（选择结束位置）

    // 兼容性字段：归一化后的起始和结束位置
    int startBlock = -1;        ///< 归一化后的起始块索引
    int startOffset = 0;         ///< 归一化后的起始偏移
    int endBlock = -1;          ///< 归一化后的结束块索引
    int endOffset = 0;           ///< 归一化后的结束偏移

    /**
     * @brief 判断选区是否为空
     * @return 如果 anchor 和 focus 在同一位置返回 true
     */
    bool isEmpty() const {
        return anchorBlock == focusBlock && anchorOffset == focusOffset;
    }

    /**
     * @brief 归一化选区（计算 start 和 end）
     * 确保 start 始终在 end 之前
     */
    void normalize() {
        if (anchorBlock > focusBlock || (anchorBlock == focusBlock && anchorOffset > focusOffset)) {
            startBlock = focusBlock;
            startOffset = focusOffset;
            endBlock = anchorBlock;
            endOffset = anchorOffset;
        } else {
            startBlock = anchorBlock;
            startOffset = anchorOffset;
            endBlock = focusBlock;
            endOffset = focusOffset;
        }
    }

    /**
     * @brief 获取焦点位置（最后一个选中字符的位置）
     * @return 焦点位置
     */
    CursorPosition focusPosition() const {
        return {focusBlock, focusOffset};
    }

    /**
     * @brief 获取锚点位置
     * @return 锚点位置
     */
    CursorPosition anchorPosition() const {
        return {anchorBlock, anchorOffset};
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
    
    /**
     * @brief 获取选区的焦点位置（最后一个选中字符的位置）
     * @return 焦点位置
     */
    CursorPosition focusPosition() const;
    
    /**
     * @brief 获取选区的锚点位置（选择开始的位置）
     * @return 锚点位置
     */
    CursorPosition anchorPosition() const;

signals:
    void selectionChanged();

private:
    Document *m_document;
    QList<SelectionRange> m_ranges;
};

} // namespace QtWordEditor

Q_DECLARE_METATYPE(QtWordEditor::SelectionRange)

#endif // SELECTION_H