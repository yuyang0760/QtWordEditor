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

private slots:
    void toggleBlink();

private:
    QTimer m_blinkTimer;
    bool m_visible;
};

} // namespace QtWordEditor

#endif // CURSORITEM_H