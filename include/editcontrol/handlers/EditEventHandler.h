#ifndef EDITEVENTHANDLER_H
#define EDITEVENTHANDLER_H

#include <QObject>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QInputMethodEvent>
#include "core/Global.h"

namespace QtWordEditor {

class Document;
class Cursor;
class Selection;

/**
 * @brief The EditEventHandler class processes user input events and translates them
 * into editing commands.
 */
class EditEventHandler : public QObject
{
    Q_OBJECT
public:
    explicit EditEventHandler(Document *document, Cursor *cursor, Selection *selection,
                              QObject *parent = nullptr);
    ~EditEventHandler() override;

    // Event handlers
    bool handleKeyPress(QKeyEvent *event);
    bool handleMousePress(QMouseEvent *event);
    bool handleMouseMove(QMouseEvent *event);
    bool handleMouseRelease(QMouseEvent *event);
    bool handleInputMethod(QInputMethodEvent *event);

private:
    Document *m_document;
    Cursor *m_cursor;
    Selection *m_selection;
};

} // namespace QtWordEditor

#endif // EDITEVENTHANDLER_H