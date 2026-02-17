#ifndef DOCUMENTVIEW_H
#define DOCUMENTVIEW_H

#include <QGraphicsView>
#include "core/Global.h"

namespace QtWordEditor {

class DocumentScene;

/**
 * @brief The DocumentView class displays the document scene and handles zooming,
 * scrolling, and event forwarding.
 */
class DocumentView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit DocumentView(QWidget *parent = nullptr);
    ~DocumentView() override;

    void setScene(DocumentScene *scene);

    qreal zoom() const;
    void setZoom(qreal zoom);
    void zoomIn();
    void zoomOut();
    void zoomToFit();

signals:
    void keyPressed(QKeyEvent *event);
    void mousePressed(QMouseEvent *event);
    void mouseMoved(QMouseEvent *event);
    void mouseReleased(QMouseEvent *event);
    void inputMethodReceived(QInputMethodEvent *event);
    void mousePositionChanged(const QPointF &scenePos, const QPoint &viewPos);
    void zoomChanged(qreal zoom);

public:
    void updateMousePosition();

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void inputMethodEvent(QInputMethodEvent *event) override;
    void scrollContentsBy(int dx, int dy) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    qreal m_zoom;
    QPoint m_lastMousePos;
};

} // namespace QtWordEditor

#endif // DOCUMENTVIEW_H