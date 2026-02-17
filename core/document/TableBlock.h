#ifndef TABLEBLOCK_H
#define TABLEBLOCK_H

#include "Block.h"
#include <QVector>
#include "core/Global.h"

namespace QtWordEditor {

class Block;

/**
 * @brief The TableBlock class represents a table block.
 *
 * It contains a grid of cells, each cell can hold any block (paragraph, image, etc.).
 */
class TableBlock : public Block
{
    Q_OBJECT
public:
    explicit TableBlock(int rows = 0, int columns = 0, QObject *parent = nullptr);
    TableBlock(const TableBlock &other);
    ~TableBlock() override;

    // Dimensions
    int rowCount() const;
    int columnCount() const;
    void setDimensions(int rows, int columns);

    // Cell content (ownership is transferred to the table)
    Block *cellContent(int row, int column) const;
    void setCellContent(int row, int column, Block *content);

    // Merge cells
    void mergeCells(int row, int column, int rowSpan, int colSpan);

    // Overrides from Block
    int length() const override;
    bool isEmpty() const override;
    Block *clone() const override;

private:
    int m_rows = 0;
    int m_columns = 0;
    QVector<QVector<Block*>> m_cells; // owning pointers
};

} // namespace QtWordEditor

#endif // TABLEBLOCK_H