#ifndef BLOCK_H
#define BLOCK_H

#include <QObject>
#include <QRectF>
#include "core/Global.h"

namespace QtWordEditor {

/**
 * @brief 块基类，是所有内容块的抽象基类
 *
 * 块是文档的基本组成单元，可以是段落、图片、表格等各种内容类型。
 * 该类定义了所有块共有的属性和接口，包括几何信息、位置管理和基本操作。
 * 具体的内容块类型需要继承此类并实现相应的虚函数。
 */
class Block : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief 构造函数
     * @param parent 父对象指针，默认为nullptr
     */
    explicit Block(QObject *parent = nullptr);
    
    /**
     * @brief 析构函数
     */
    ~Block() override;

    // ========== 标识相关方法 ==========
    
    /**
     * @brief 获取块ID
     * @return 块的唯一标识符
     */
    int blockId() const;
    
    /**
     * @brief 设置块ID
     * @param id 新的块标识符
     */
    void setBlockId(int id);

    // ========== 几何属性相关方法（由布局引擎设置）==========
    
    /**
     * @brief 获取块的边界矩形
     * @return 场景坐标系中的边界矩形
     */
    QRectF boundingRect() const;
    
    /**
     * @brief 设置块的边界矩形
     * @param rect 新的边界矩形
     */
    void setBoundingRect(const QRectF &rect);

    /**
     * @brief 获取块的高度
     * @return 块的高度值
     */
    qreal height() const;
    
    /**
     * @brief 设置块的高度
     * @param height 新的高度值
     */
    void setHeight(qreal height);

    // ========== 文档中位置相关方法（由Document管理的全局索引）==========
    
    /**
     * @brief 获取块在文档中的全局位置
     * @return 全局块索引
     */
    int positionInDocument() const;
    
    /**
     * @brief 设置块在文档中的全局位置
     * @param pos 新的全局块索引
     */
    void setPositionInDocument(int pos);

    // ========== 纯虚函数（子类必须实现）==========
    
    /**
     * @brief 获取块的内容长度
     * @return 块中内容的长度
     */
    virtual int length() const = 0;
    
    /**
     * @brief 判断块是否为空
     * @return 如果块为空则返回true，否则返回false
     */
    virtual bool isEmpty() const = 0;
    
    /**
     * @brief 克隆当前块
     * @return 指向克隆块的指针
     */
    virtual Block *clone() const = 0;

signals:
    /** @brief 块的几何属性发生变化时发出的信号 */
    void geometryChanged();

private:
    int m_blockId = -1;              ///< 块唯一标识符
    QRectF m_boundingRect;           ///< 块的边界矩形
    qreal m_height = 0.0;            ///< 块的高度
    int m_positionInDocument = -1;   ///< 块在文档中的全局位置索引
};

} // namespace QtWordEditor

#endif // BLOCK_H