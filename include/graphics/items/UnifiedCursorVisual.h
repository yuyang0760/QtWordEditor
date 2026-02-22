#ifndef UNIFIEDCURSORVISUAL_H
#define UNIFIEDCURSORVISUAL_H

#include <QGraphicsItem>
#include <QObject>
#include <QTimer>
#include "core/Global.h"

namespace QtWordEditor {

/**
 * @brief 统一光标视觉组件
 * 
 * 提供统一的光标视觉渲染，支持：
 * - 作为任意 QGraphicsItem 的子项
 * - 动态设置高度和位置
 * - 统一的闪烁和可见性控制
 */
class UnifiedCursorVisual : public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)
    
public:
    enum { Type = UserType + 2200 };
    
    /**
     * @brief 构造函数
     * @param parent 父图形项
     */
    explicit UnifiedCursorVisual(QGraphicsItem *parent = nullptr);
    
    /**
     * @brief 析构函数
     */
    ~UnifiedCursorVisual() override;
    
    // ========== 位置和尺寸设置 ==========
    
    /**
     * @brief 设置光标位置和高度
     * @param pos 光标位置（相对于父项）
     * @param height 光标高度
     */
    void setPosition(const QPointF &pos, qreal height);
    
    /**
     * @brief 设置光标高度
     * @param height 高度值
     */
    void setHeight(qreal height);
    
    // ========== 可见性控制 ==========
    
    /**
     * @brief 设置光标可见性
     * @param visible 是否可见
     */
    void setVisible(bool visible);
    
    /**
     * @brief 获取光标可见性
     * @return 是否可见
     */
    bool isVisible() const;
    
    // ========== 闪烁控制 ==========
    
    /**
     * @brief 开始闪烁
     */
    void startBlink();
    
    /**
     * @brief 停止闪烁
     */
    void stopBlink();
    
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
    qreal m_height;          ///< 光标高度
    bool m_visible;           ///< 是否可见
    bool m_blinkState;        ///< 闪烁状态
    QTimer *m_blinkTimer;     ///< 闪烁定时器
};

} // namespace QtWordEditor

#endif // UNIFIEDCURSORVISUAL_H
