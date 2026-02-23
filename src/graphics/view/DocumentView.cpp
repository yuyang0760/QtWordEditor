#include "graphics/view/DocumentView.h"
#include "graphics/scene/DocumentScene.h"
#include "editcontrol/cursor/Cursor.h"
#include "graphics/cursor/UnifiedCursor.h"
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
    , m_unifiedCursor(nullptr)
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
    qDebug() << "[DEBUG] DocumentView::keyPressEvent - key:" << event->key() << "text:" << event->text();
    
    // 优先使用 UnifiedCursor 直接处理按键
    if (m_unifiedCursor) {
        bool handled = false;
        switch (event->key()) {
        case Qt::Key_Left:
            qDebug() << "[DEBUG] DocumentView - 处理左箭头";
            m_unifiedCursor->moveLeft();
            handled = true;
            break;
        case Qt::Key_Right:
            qDebug() << "[DEBUG] DocumentView - 处理右箭头";
            m_unifiedCursor->moveRight();
            handled = true;
            break;
        case Qt::Key_Up:
            qDebug() << "[DEBUG] DocumentView - 处理上箭头";
            m_unifiedCursor->moveUp();
            handled = true;
            break;
        case Qt::Key_Down:
            qDebug() << "[DEBUG] DocumentView - 处理下箭头";
            m_unifiedCursor->moveDown();
            handled = true;
            break;
        case Qt::Key_Home:
            qDebug() << "[DEBUG] DocumentView - 处理 Home";
            m_unifiedCursor->moveToStartOfLine();
            handled = true;
            break;
        case Qt::Key_End:
            qDebug() << "[DEBUG] DocumentView - 处理 End";
            m_unifiedCursor->moveToEndOfLine();
            handled = true;
            break;
        case Qt::Key_Backspace:
            qDebug() << "[DEBUG] DocumentView - 处理 Backspace";
            m_unifiedCursor->deletePreviousChar();
            handled = true;
            break;
        case Qt::Key_Delete:
            qDebug() << "[DEBUG] DocumentView - 处理 Delete";
            m_unifiedCursor->deleteChar();
            handled = true;
            break;
        default:
            if (!event->text().isEmpty()) {
                qDebug() << "[DEBUG] DocumentView - 处理输入字符:" << event->text();
                m_unifiedCursor->insertText(event->text());
                handled = true;
            }
            break;
        }

        if (handled) {
            qDebug() << "[DEBUG] DocumentView - 事件已处理，接受";
            event->accept();
            return;
        }
    }

    qDebug() << "[DEBUG] DocumentView - 回退到旧的事件处理";
    // 如果 UnifiedCursor 不可用，回退到旧的事件处理
    emit keyPressed(event);
    QGraphicsView::keyPressEvent(event);
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

void DocumentView::keyReleaseEvent(QKeyEvent *event)
{
    // Forward if needed
    QGraphicsView::keyReleaseEvent(event);
}

void DocumentView::mousePressEvent(QMouseEvent *event)
{
    QPointF scenePos = mapToScene(event->pos());
    
    // 优先使用 UnifiedCursor 直接处理鼠标按下
    if (m_unifiedCursor) {
        m_unifiedCursor->setPositionFromScenePoint(scenePos);
        event->accept();
        return;
    }
    
    // 如果 UnifiedCursor 不可用，回退到旧的事件处理
    emit mousePressed(scenePos);
    QGraphicsView::mousePressEvent(event);
}

void DocumentView::mouseMoveEvent(QMouseEvent *event)
{
    QPointF scenePos = mapToScene(event->pos());
    
    // 鼠标移动时，暂时不更新光标位置（除非是选择模式，以后再完善）
    // 优先使用 UnifiedCursor，但只更新鼠标位置信号，不更新光标位置
    if (!m_unifiedCursor) {
        // 如果 UnifiedCursor 不可用，回退到旧的事件处理
        emit mouseMoved(scenePos);
    }
    
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
    
    // 优先使用 UnifiedCursor 直接处理鼠标释放（目前暂时不需要额外处理）
    if (m_unifiedCursor) {
        event->accept();
        return;
    }
    
    // 如果 UnifiedCursor 不可用，回退到旧的事件处理
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
    // 优先使用 UnifiedCursor 直接处理输入法
    if (m_unifiedCursor && !event->commitString().isEmpty()) {
        m_unifiedCursor->insertText(event->commitString());
        event->accept();
        return;
    }
    
    // 如果 UnifiedCursor 不可用，回退到旧的处理
    QGraphicsView::inputMethodEvent(event);
}

void DocumentView::setUnifiedCursor(UnifiedCursor *cursor)
{
    m_unifiedCursor = cursor;
}

UnifiedCursor* DocumentView::unifiedCursor() const
{
    return m_unifiedCursor;
}

QVariant DocumentView::inputMethodQuery(Qt::InputMethodQuery query) const
{
    switch (query) {
    case Qt::ImEnabled:
        return true;
    case Qt::ImCursorRectangle:
    {
        // 优先使用 UnifiedCursor 的光标位置
        if (m_unifiedCursor && m_unifiedCursor->cursorItem()) {
            QRectF cursorRect = m_unifiedCursor->cursorItem()->rect();
            QPointF scenePos = m_unifiedCursor->cursorItem()->mapToScene(cursorRect.topLeft());
            QPoint viewPos = mapFromScene(scenePos);
            return QRect(viewPos, QSize(2, static_cast<int>(cursorRect.height())));
        } else {
            // 旧系统的光标位置
            QPoint viewPos = mapFromScene(m_cursorVisualPos);
            return QRect(viewPos, QSize(1, 20));
        }
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