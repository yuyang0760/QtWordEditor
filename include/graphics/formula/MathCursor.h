/**
 * @file MathCursor.h
 * @brief 数学公式光标类
 * 
 * 光标在公式内部的 MathItem 树中导航，提供闪烁的视觉指示器。
 */

#ifndef MATHCURSOR_H
#define MATHCURSOR_H

#include <QGraphicsItem>
#include <QObject>
#include <QTimer>
#include "core/Global.h"

namespace QtWordEditor {

class MathItem;
class RowContainerItem;

/**
 * @brief 数学公式光标类
 * 
 * 光标在公式内部的 MathItem 树中导航，提供闪烁的视觉指示器。
 * 使用 QObject 来支持信号槽机制。
 */
class MathCursor : public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)
    
public:
    enum { Type = UserType + 2100 };
    
    /**
     * @brief 构造函数
     * @param parent 父图形项
     */
    explicit MathCursor(QGraphicsItem *parent = nullptr);
    
    /**
     * @brief 析构函数
     */
    ~MathCursor() override;
    
    // ========== 光标位置管理 ==========
    
    /**
     * @brief 设置光标位置
     * @param container 光标所在的行容器
     * @param position 光标在容器中的位置索引
     */
    void setPosition(RowContainerItem *container, int position);
    
    /**
     * @brief 获取当前所在的行容器
     * @return 行容器指针
     */
    RowContainerItem *currentContainer() const;
    
    /**
     * @brief 获取光标在容器中的位置
     * @return 位置索引
     */
    int position() const;
    
    // ========== 光标导航 ==========
    
    /**
     * @brief 向左移动光标
     */
    void moveLeft();
    
    /**
     * @brief 向右移动光标
     */
    void moveRight();
    
    /**
     * @brief 向上移动光标
     */
    void moveUp();
    
    /**
     * @brief 向下移动光标
     */
    void moveDown();
    
    /**
     * @brief 移动到父容器
     */
    void moveToParent();
    
    // ========== 视觉控制 ==========
    
    /**
     * @brief 设置光标可见性
     * @param visible 是否可见
     */
    void setVisible(bool visible);
    
    /**
     * @brief 设置光标高度
     * @param height 高度值
     */
    void setHeight(qreal height);
    
    /**
     * @brief 更新视觉位置
     */
    void updateVisualPosition();
    
    // ========== Qt 标准接口 ==========
    
    int type() const override { return Type; }
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    
private slots:
    /**
     * @brief 闪烁定时器回调
     */
    void blink();
    
private:
    RowContainerItem *m_currentContainer;  ///< 当前所在的行容器
    int m_position;                          ///< 光标在容器中的位置
    qreal m_height;                          ///< 光标高度
    bool m_visible;                          ///< 是否可见
    bool m_blinkState;                       ///< 闪烁状态
    QTimer *m_blinkTimer;                    ///< 闪烁定时器
};

} // namespace QtWordEditor

#endif // MATHCURSOR_H
