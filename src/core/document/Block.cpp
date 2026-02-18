
/**
 * @file Block.cpp
 * @brief 块基类的实现文件
 *
 * 此文件包含了块基类的实现，该类是文档中所有内容块类型的抽象基类，
 * 定义了块的基本属性和通用操作方法。
 */

#include "core/document/Block.h"
#include <QDebug>

namespace QtWordEditor {

/**
 * @brief 构造块对象
 * @param parent 父QObject对象
 */
Block::Block(QObject *parent)
    : QObject(parent)
{
}

/**
 * @brief 销毁块对象
 */
Block::~Block()
{
}

/**
 * @brief 获取块ID
 * @return 块的标识符
 */
int Block::blockId() const
{
    return m_blockId;
}

/**
 * @brief 设置块ID
 * @param id 新的块标识符
 */
void Block::setBlockId(int id)
{
    if (m_blockId != id) {
        m_blockId = id;
    }
}

/**
 * @brief 获取块的边界矩形
 * @return 场景坐标系中的边界矩形
 */
QRectF Block::boundingRect() const
{
    return m_boundingRect;
}

/**
 * @brief Sets the bounding rectangle of the block
 * @param rect New bounding rectangle
 */
void Block::setBoundingRect(const QRectF &rect)
{
    if (m_boundingRect != rect) {
        m_boundingRect = rect;
        emit geometryChanged();
    }
}

/**
 * @brief 获取块的高度
 * @return 块的高度值
 */
qreal Block::height() const
{
    return m_height;
}

/**
 * @brief 设置块的高度
 * @param height 新的高度值
 */
void Block::setHeight(qreal height)
{
    if (!qFuzzyCompare(m_height, height)) {
        m_height = height;
        emit geometryChanged();
    }
}

/**
 * @brief 获取块在文档中的全局位置
 * @return 全局块索引
 */
int Block::positionInDocument() const
{
    return m_positionInDocument;
}

/**
 * @brief 设置块在文档中的全局位置
 * @param pos 新的全局块索引
 */
void Block::setPositionInDocument(int pos)
{
    if (m_positionInDocument != pos) {
        m_positionInDocument = pos;
    }
}

} // namespace QtWordEditor

