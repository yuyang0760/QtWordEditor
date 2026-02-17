#include "graphics/view/DocumentView.h"
#include "graphics/scene/DocumentScene.h"
#include "editcontrol/cursor/Cursor.h"
#include <QKeyEvent>
#include <QWheelEvent>
#include <QInputMethodEvent>
#include <QInputMethod>
#include <QGuiApplication>
#include <QDebug>

namespace QtWordEditor {

DocumentView::DocumentView(QWidget *parent)
    : QGraphicsView(parent)
    , m_zoom(100.0)
    , m_lastMousePos(-1, -1)
    , m_cursor(nullptr)
    , m_cursorVisualPos(0, 0)
{
    setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    setDragMode(QGraphicsView::NoDrag);
    setMouseTracking(true);
    
    // 启用输入法支持
    setAttribute(Qt::WA_InputMethodEnabled, true);
    setFocusPolicy(Qt::StrongFocus);
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
    emit zoomChanged(m_zoom);
    updateMousePosition();
}

void DocumentView::updateMousePosition()
{
    if (m_lastMousePos.x() >= 0 && m_lastMousePos.y() >= 0) {
        QPointF scenePos = mapToScene(m_lastMousePos);
        emit mousePositionChanged(scenePos, m_lastMousePos);
    }
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
    QPointF scenePos = mapToScene(event->pos());
    emit mousePressed(scenePos);
    QGraphicsView::mousePressEvent(event);
}

void DocumentView::mouseMoveEvent(QMouseEvent *event)
{
    emit mouseMoved(event);
    
    m_lastMousePos = event->pos();
    QPointF scenePos = mapToScene(event->pos());
    QPoint viewPos = event->pos();
    emit mousePositionChanged(scenePos, viewPos);
    
    QGraphicsView::mouseMoveEvent(event);
}

void DocumentView::scrollContentsBy(int dx, int dy)
{
    QGraphicsView::scrollContentsBy(dx, dy);
    updateMousePosition();
}

void DocumentView::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);
    updateMousePosition();
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

void DocumentView::setCursor(Cursor *cursor)
{
    m_cursor = cursor;
}

void DocumentView::setCursorVisualPosition(const QPointF &pos)
{
    m_cursorVisualPos = pos;
    QWidget::update();
    if (QInputMethod *inputMethod = QGuiApplication::inputMethod()) {
        inputMethod->update(Qt::ImCursorRectangle);
    }
}

QVariant DocumentView::inputMethodQuery(Qt::InputMethodQuery query) const
{
    switch (query) {
    case Qt::ImEnabled:
        return true;
    case Qt::ImCursorRectangle:
    {
        // 将场景坐标转换为视图坐标
        QPoint viewPos = mapFromScene(m_cursorVisualPos);
        // 返回一个合适大小的矩形，输入法候选框会显示在这个位置下方
        return QRect(viewPos, QSize(1, 20));
    }
    case Qt::ImCursorPosition:
        return 0;
    case Qt::ImSurroundingText:
        return QString();
    case Qt::ImCurrentSelection:
        return QString();
    case Qt::ImMaximumTextLength:
        return -1;
    case Qt::ImAnchorPosition:
        return 0;
    default:
        return QGraphicsView::inputMethodQuery(query);
    }
}

} // namespace QtWordEditor