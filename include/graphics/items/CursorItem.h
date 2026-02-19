#ifndef CURSORITEM_H
#define CURSORITEM_H

#include <QGraphicsLineItem>
#include <QTimer>
#include <QObject>
#include "core/Global.h"

namespace QtWordEditor {

/**
 * @brief The CursorItem class draws a blinking vertical line as the text cursor.
 */
class CursorItem : public QGraphicsLineItem
{
public:
    explicit CursorItem(QGraphicsItem *parent = nullptr);
    ~CursorItem() override;

    void setPosition(const QPointF &pos, qreal height);
    void startBlink();
    void stopBlink();
    
    // ========== 新增方法：控制光标可见性 ==========
    // 设置光标是否可见（true=显示，false=隐藏）
    // 隐藏时会停止闪烁
    void setCursorVisible(bool visible);

private slots:
    void toggleBlink();

private:
    QTimer m_blinkTimer;
    bool m_visible;
    bool m_cursorEnabled;  // 新增变量：光标是否启用
};

} // namespace QtWordEditor

#endif // CURSORITEM_H