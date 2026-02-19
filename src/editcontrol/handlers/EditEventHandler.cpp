#include "editcontrol/handlers/EditEventHandler.h"
#include "core/document/Document.h"
#include "editcontrol/cursor/Cursor.h"
#include "editcontrol/selection/Selection.h"
#include "editcontrol/formatting/FormatController.h"
#include "graphics/scene/DocumentScene.h"
#include <QDebug>

namespace QtWordEditor {

EditEventHandler::EditEventHandler(Document *document, Cursor *cursor, Selection *selection,
                                   FormatController *formatController,
                                   QObject *parent)
    : QObject(parent)
    , m_document(document)
    , m_cursor(cursor)
    , m_selection(selection)
    , m_formatController(formatController)
    , m_scene(nullptr)
    , m_isSelecting(false)
    , m_selectionStartBlock(0)
    , m_selectionStartOffset(0)
{
}

EditEventHandler::~EditEventHandler()
{
}

void EditEventHandler::setScene(DocumentScene *scene)
{
    m_scene = scene;
}

bool EditEventHandler::handleKeyPress(QKeyEvent *event)
{
    if (!m_document || !m_cursor || !m_selection)
        return false;

    bool handled = false;
    switch (event->key()) {
    case Qt::Key_Left:
        if (event->modifiers() & Qt::ShiftModifier) {
            // Extend selection left
            // TODO: implement
        } else {
            m_cursor->moveLeft();
        }
        handled = true;
        break;
    case Qt::Key_Right:
        if (event->modifiers() & Qt::ShiftModifier) {
            // Extend selection right
        } else {
            m_cursor->moveRight();
        }
        handled = true;
        break;
    case Qt::Key_Up:
        m_cursor->moveUp();
        handled = true;
        break;
    case Qt::Key_Down:
        m_cursor->moveDown();
        handled = true;
        break;
    case Qt::Key_Home:
        if (event->modifiers() & Qt::ShiftModifier) {
            // Extend selection to start of line
        } else {
            m_cursor->moveToStartOfLine();
        }
        handled = true;
        break;
    case Qt::Key_End:
        if (event->modifiers() & Qt::ShiftModifier) {
            // Extend selection to end of line
        } else {
            m_cursor->moveToEndOfLine();
        }
        handled = true;
        break;
    case Qt::Key_Backspace:
        m_cursor->deletePreviousChar();
        handled = true;
        break;
    case Qt::Key_Delete:
        m_cursor->deleteNextChar();
        handled = true;
        break;
    case Qt::Key_Return:
    case Qt::Key_Enter:
        // Insert new paragraph block
        // TODO: implement
        break;
    default:
        // Typed character
        if (!event->text().isEmpty()) {
            CharacterStyle style;
            if (m_formatController) {
                style = m_formatController->getCurrentInputStyle();
            }
            m_cursor->insertText(event->text(), style);
            handled = true;
        }
        break;
    }

    return handled;
}

bool EditEventHandler::handleMousePress(const QPointF &scenePos)
{
    if (!m_scene || !m_cursor || !m_selection)
        return false;

  //  QDebug() << "EditEventHandler::handleMousePress at:" << scenePos;

    // 获取光标位置
    CursorPosition cursorPos = m_scene->cursorPositionAt(scenePos);

    // 设置光标位置
    m_cursor->setPosition(cursorPos);

    // 开始选择
    m_isSelecting = true;
    m_selectionStartBlock = cursorPos.blockIndex;
    m_selectionStartOffset = cursorPos.offset;

    // 清除之前的选择
    m_selection->clear();

    // 发送信号更新选择显示
    emit selectionNeedsUpdate();

    return true;
}

bool EditEventHandler::handleMouseMove(const QPointF &scenePos)
{
    if (!m_scene || !m_cursor || !m_selection || !m_isSelecting)
        return false;

    qDebug() << "EditEventHandler::handleMouseMove at:" << scenePos;

    // 获取光标位置
    CursorPosition cursorPos = m_scene->cursorPositionAt(scenePos);

    qDebug() << "  光标位置: 块" << cursorPos.blockIndex << "，偏移" << cursorPos.offset;
    qDebug() << "  选择起始: 块" << m_selectionStartBlock << "，偏移" << m_selectionStartOffset;

    // 更新选择范围
    m_selection->setRange(
        m_selectionStartBlock,
        m_selectionStartOffset,
        cursorPos.blockIndex,
        cursorPos.offset
    );

    qDebug() << "  选择范围已设置";

    // 更新光标位置（到选择的终点）
    m_cursor->setPosition(cursorPos);

    // 发送信号更新选择显示
    emit selectionNeedsUpdate();

    return true;
}

bool EditEventHandler::handleMouseRelease(const QPointF &scenePos)
{
    if (!m_scene || !m_cursor || !m_selection)
        return false;

    qDebug() << "EditEventHandler::handleMouseRelease at:" << scenePos;

    // 获取最终的选择范围
    SelectionRange range = m_selection->range();
    qDebug() << "  最终选择范围:";
    qDebug() << "    Anchor: 块" << range.anchorBlock << "，偏移" << range.anchorOffset;
    qDebug() << "    Focus: 块" << range.focusBlock << "，偏移" << range.focusOffset;
    qDebug() << "    Start: 块" << range.startBlock << "，偏移" << range.startOffset;
    qDebug() << "    End: 块" << range.endBlock << "，偏移" << range.endOffset;

    // 结束选择
    m_isSelecting = false;

    // 发出选择完成信号，用于更新工具栏样式
    emit selectionFinished();

    return true;
}

bool EditEventHandler::handleInputMethod(QInputMethodEvent *event)
{
    if (!m_document || !m_cursor)
        return false;

  //  QDebug() << "EditEventHandler::handleInputMethod called";
  //  QDebug() << "  commitString:" << event->commitString();
  //  QDebug() << "  preeditString:" << event->preeditString();
    
    if (!event->commitString().isEmpty()) {
        CharacterStyle style;
        if (m_formatController) {
            style = m_formatController->getCurrentInputStyle();
        }
        m_cursor->insertText(event->commitString(), style);
        return true;
    }
    
    return false;
}

} // namespace QtWordEditor