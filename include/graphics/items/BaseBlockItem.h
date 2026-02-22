#ifndef BASEBLOCKITEM_H
#define BASEBLOCKITEM_H

#include <QGraphicsItem>
#include <QRectF>
#include "core/Global.h"

namespace QtWordEditor {

class Block;

/**
 * @brief 块图形项基类，是所有块图形项的基类
 *
 * 该类继承自QGraphicsItem，作为文档中各种块类型
 * （如文本块、图片块、表格块等）在图形场景中的可视化表示基类。
 * 每个具体的块图形项都需要继承此类并实现相应的更新逻辑。
 */
class BaseBlockItem : public QGraphicsItem
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

    // ========== QGraphicsItem 接口实现（为了兼容旧代码） ==========
    /**
     * @brief 获取边界矩形
     * @return 边界矩形
     */
    QRectF boundingRect() const override;
    
    /**
     * @brief 绘制图形项
     * @param painter 绘制器
     * @param option 样式选项
     * @param widget 窗口部件
     */
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    // ========== 兼容旧 QGraphicsRectItem 接口的方法 ==========
    /**
     * @brief 设置矩形
     * @param rect 矩形
     */
    void setRect(const QRectF &rect);
    
    /**
     * @brief 设置矩形（重载）
     * @param x x坐标
     * @param y y坐标
     * @param w 宽度
     * @param h 高度
     */
    void setRect(qreal x, qreal y, qreal w, qreal h);
    
    /**
     * @brief 获取矩形
     * @return 矩形
     */
    QRectF rect() const;

protected:
    Block *m_block;       ///< 关联的数据块对象
    QRectF m_boundingRect; ///< 边界矩形（兼容旧代码）
};

} // namespace QtWordEditor

#endif // BASEBLOCKITEM_H