#include "graphics/view/DocumentView.h"
#include "graphics/scene/DocumentScene.h"
#include "editcontrol/cursor/Cursor.h"
#include "graphics/items/TextBlockItem.h"
#include <QKeyEvent>
#include <QWheelEvent>
#include <QInputMethodEvent>
#include <QInputMethod>
#include <QGuiApplication>
#include <QDebug>
#include <QMenu>
#include <QContextMenuEvent>

namespace QtWordEditor {

DocumentView::DocumentView(QWidget *parent)
    : QGraphicsView(parent)
    , m_zoom(100.0)
    , m_lastMousePos(-1, -1)
    , m_cursor(nullptr)
    , m_cursorVisualPos(0, 0)
{
    // 使用默认的软件渲染视口（不使用GPU加速）
    setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    // 使用智能视口更新而不是全屏更新，提高性能
    setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);
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
    qDebug() << "[DocumentView::keyPressEvent] key:" << event->key() << ", text:" << event->text();
    
    // ========== 检查是否有 TextBlockItem 处于公式编辑模式 ==========
    bool isInMathEditMode = false;
    if (scene()) {
        QList<QGraphicsItem *> items = scene()->items();
        for (QGraphicsItem *item : items) {
            TextBlockItem *textBlockItem = dynamic_cast<TextBlockItem *>(item);
            if (textBlockItem && textBlockItem->isInMathEditMode()) {
                qDebug() << "[DocumentView] TextBlockItem 处于公式编辑模式";
                isInMathEditMode = true;
                break;
            }
        }
    }
    
    // ========== 先让 QGraphicsScene 处理键盘事件，这样 TextBlockItem 会收到 ==========
    QGraphicsView::keyPressEvent(event);
    
    // ========== 如果不是在公式编辑模式，才发送信号让 EditEventHandler 处理 ==========
    if (!isInMathEditMode) {
        emit keyPressed(event);
    } else {
        qDebug() << "[DocumentView] 在公式编辑模式，不发送信号给 EditEventHandler";
    }
}

void DocumentView::keyReleaseEvent(QKeyEvent *event)
{
    // Forward if needed
    QGraphicsView::keyReleaseEvent(event);
}

void DocumentView::mousePressEvent(QMouseEvent *event)
{
    QPointF scenePos = mapToScene(event->pos());
    qDebug() << "[DocumentView::mousePressEvent] scenePos:" << scenePos;
    
    // ========== 先让 QGraphicsScene 处理鼠标事件，这样 TextBlockItem 会收到 ==========
    QGraphicsView::mousePressEvent(event);
    
    // ========== 再发送信号让 EditEventHandler 处理 ==========
    emit mousePressed(scenePos);
}

void DocumentView::mouseMoveEvent(QMouseEvent *event)
{
    QPointF scenePos = mapToScene(event->pos());
    emit mouseMoved(scenePos);
    
    m_lastMousePos = event->pos();
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
    QPointF scenePos = mapToScene(event->pos());
    emit mouseReleased(scenePos);
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

void DocumentView::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu(this);
    
    // 添加段落设置菜单项
    QAction *paragraphAction = menu.addAction(tr("段落(&P)..."));
    connect(paragraphAction, &QAction::triggered, this, &DocumentView::contextMenuParagraphRequested);
    
    // 在鼠标位置显示菜单
    menu.exec(event->globalPos());
}

} // namespace QtWordEditor