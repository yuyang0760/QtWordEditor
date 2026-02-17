#include "editcontrol/handlers/EditEventHandler.h"
#include "core/document/Document.h"
#include "editcontrol/cursor/Cursor.h"
#include "editcontrol/selection/Selection.h"
#include <QDebug>

namespace QtWordEditor {

EditEventHandler::EditEventHandler(Document *document, Cursor *cursor, Selection *selection,
                                   QObject *parent)
    : QObject(parent)
    , m_document(document)
    , m_cursor(cursor)
    , m_selection(selection)
{
}

EditEventHandler::~EditEventHandler()
{
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
            CharacterStyle style; // default style
            m_cursor->insertText(event->text(), style);
            handled = true;
        }
        break;
    }

    return handled;
}

bool EditEventHandler::handleMousePress(QMouseEvent *event)
{
    Q_UNUSED(event);
    // TODO: implement mouse selection
    return false;
}

bool EditEventHandler::handleMouseMove(QMouseEvent *event)
{
    Q_UNUSED(event);
    return false;
}

bool EditEventHandler::handleMouseRelease(QMouseEvent *event)
{
    Q_UNUSED(event);
    return false;
}

bool EditEventHandler::handleInputMethod(QInputMethodEvent *event)
{
    if (!m_document || !m_cursor)
        return false;

    qDebug() << "EditEventHandler::handleInputMethod called";
    qDebug() << "  commitString:" << event->commitString();
    qDebug() << "  preeditString:" << event->preeditString();
    
    if (!event->commitString().isEmpty()) {
        CharacterStyle style;
        m_cursor->insertText(event->commitString(), style);
        return true;
    }
    
    return false;
}

} // namespace QtWordEditor