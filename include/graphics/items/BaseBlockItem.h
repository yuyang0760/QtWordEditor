#ifndef BASEBLOCKITEM_H
#define BASEBLOCKITEM_H

#include <QGraphicsRectItem>
#include "core/Global.h"

namespace QtWordEditor {

class Block;

/**
 * @brief 块图形项基类，是所有块图形项的基类
 *
 * 该类继承自QGraphicsRectItem，作为文档中各种块类型
 * （如文本块、图片块、表格块等）在图形场景中的可视化表示基类。
 * 每个具体的块图形项都需要继承此类并实现相应的更新逻辑。
 */
class BaseBlockItem : public QGraphicsRectItem
{
public:
    /**
     * @brief 构造函数
     * @param block 关联的数据块对象
     * @param parent 父图形项指针，默认为nullptr
     */
    explicit BaseBlockItem(Block *block, QGraphicsItem *parent = nullptr);
    
    /**
     * @brief 析构函数
     */
    ~BaseBlockItem() override;

    /**
     * @brief 获取关联的数据块
     * @return 指向关联块对象的指针
     */
    Block *block() const;
    
    /**
     * @brief 更新图形项显示
     * 纯虚函数，子类必须实现具体的更新逻辑
     */
    virtual void updateBlock() = 0;

protected:
    Block *m_block;  ///< 关联的数据块对象
};

} // namespace QtWordEditor

#endif // BASEBLOCKITEM_H