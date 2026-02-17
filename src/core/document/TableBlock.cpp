#include "core/document/TableBlock.h"
#include <QDebug>

namespace QtWordEditor {

TableBlock::TableBlock(int rows, int columns, QObject *parent)
    : Block(parent)
    , m_rows(rows)
    , m_columns(columns)
{
    m_cells.resize(rows);
    for (int r = 0; r < rows; ++r) {
        m_cells[r].resize(columns);
        for (int c = 0; c < columns; ++c) {
            m_cells[r][c] = nullptr;
        }
    }
}

TableBlock::TableBlock(const TableBlock &other)
    : Block(other.parent())
    , m_rows(other.m_rows)
    , m_columns(other.m_columns)
{
    // Deep copy of cells
    m_cells.resize(m_rows);
    for (int r = 0; r < m_rows; ++r) {
        m_cells[r].resize(m_columns);
        for (int c = 0; c < m_columns; ++c) {
            Block *orig = other.m_cells[r][c];
            m_cells[r][c] = orig ? orig->clone() : nullptr;
        }
    }
}

TableBlock::~TableBlock()
{
    for (int r = 0; r < m_rows; ++r) {
        for (int c = 0; c < m_columns; ++c) {
            delete m_cells[r][c];
        }
    }
}

int TableBlock::rowCount() const
{
    return m_rows;
}

int TableBlock::columnCount() const
{
    return m_columns;
}

void TableBlock::setDimensions(int rows, int columns)
{
    // TODO: implement resizing with content preservation
    m_rows = rows;
    m_columns = columns;
    m_cells.resize(rows);
    for (int r = 0; r < rows; ++r) {
        m_cells[r].resize(columns);
    }
}

Block *TableBlock::cellContent(int row, int column) const
{
    if (row >= 0 && row < m_rows && column >= 0 && column < m_columns)
        return m_cells[row][column];
    return nullptr;
}

void TableBlock::setCellContent(int row, int column, Block *content)
{
    if (row >= 0 && row < m_rows && column >= 0 && column < m_columns) {
        delete m_cells[row][column];
        m_cells[row][column] = content;
    }
}

void TableBlock::mergeCells(int row, int column, int rowSpan, int colSpan)
{
    // TODO: implement cell merging
    Q_UNUSED(row);
    Q_UNUSED(column);
    Q_UNUSED(rowSpan);
    Q_UNUSED(colSpan);
}

int TableBlock::length() const
{
    return 1; // Table block counts as one unit for cursor navigation.
}

bool TableBlock::isEmpty() const
{
    for (int r = 0; r < m_rows; ++r) {
        for (int c = 0; c < m_columns; ++c) {
            if (m_cells[r][c] && !m_cells[r][c]->isEmpty())
                return false;
        }
    }
    return true;
}

Block *TableBlock::clone() const
{
    return new TableBlock(*this);
}

} // namespace QtWordEditor