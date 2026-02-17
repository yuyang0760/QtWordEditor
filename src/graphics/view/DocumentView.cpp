#include "graphics/view/DocumentView.h"
#include "graphics/scene/DocumentScene.h"
#include <QKeyEvent>
#include <QWheelEvent>
#include <QInputMethodEvent>
#include <QDebug>

namespace QtWordEditor {

DocumentView::DocumentView(QWidget *parent)
    : QGraphicsView(parent)
    , m_zoom(100.0)
{
    setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
}

DocumentView::~DocumentView()
{
}

void DocumentView::setScene(DocumentScene *scene)
{
    QGraphicsView::setScene(scene);
}

qreal DocumentView::zoom() const
{
    return m_zoom;
}

void DocumentView::setZoom(qreal zoom)
{
    if (qFuzzyCompare(m_zoom, zoom))
        return;
    m_zoom = zoom;
    resetTransform();
    scale(m_zoom / 100.0, m_zoom / 100.0);
}

void DocumentView::zoomIn()
{
    setZoom(m_zoom + 10);
}

void DocumentView::zoomOut()
{
    setZoom(m_zoom - 10);
}

void DocumentView::zoomToFit()
{
    // TODO: implement fit‑to‑width or fit‑to‑page
}

void DocumentView::keyPressEvent(QKeyEvent *event)
{
    emit keyPressed(event);
    // Do not call base to avoid default handling (optional)
    // QGraphicsView::keyPressEvent(event);
}

void DocumentView::keyReleaseEvent(QKeyEvent *event)
{
    // Forward if needed
    QGraphicsView::keyReleaseEvent(event);
}

void DocumentView::mousePressEvent(QMouseEvent *event)
{
    emit mousePressed(event);
    QGraphicsView::mousePressEvent(event);
}

void DocumentView::mouseMoveEvent(QMouseEvent *event)
{
    emit mouseMoved(event);
    QGraphicsView::mouseMoveEvent(event);
}

void DocumentView::mouseReleaseEvent(QMouseEvent *event)
{
    emit mouseReleased(event);
    QGraphicsView::mouseReleaseEvent(event);
}

void DocumentView::wheelEvent(QWheelEvent *event)
{
    if (event->modifiers() & Qt::ControlModifier) {
        // Zoom with Ctrl+Wheel
        if (event->angleDelta().y() > 0)
            zoomIn();
        else
            zoomOut();
        event->accept();
    } else {
        QGraphicsView::wheelEvent(event);
    }
}

void DocumentView::inputMethodEvent(QInputMethodEvent *event)
{
    emit inputMethodReceived(event);
    QGraphicsView::inputMethodEvent(event);
}

} // namespace QtWordEditor