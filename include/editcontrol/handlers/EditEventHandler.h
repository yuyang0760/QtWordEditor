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
class DocumentScene;
class FormatController;

/**
 * @brief The EditEventHandler class processes user input events and translates them
 * into editing commands.
 */
class EditEventHandler : public QObject
{
    Q_OBJECT
public:
    explicit EditEventHandler(Document *document, Cursor *cursor, Selection *selection,
                              FormatController *formatController = nullptr,
                              QObject *parent = nullptr);
    ~EditEventHandler() override;

    // Event handlers
    bool handleKeyPress(QKeyEvent *event);
    bool handleMousePress(const QPointF &scenePos);
    bool handleMouseMove(const QPointF &scenePos);
    bool handleMouseRelease(const QPointF &scenePos);
    bool handleInputMethod(QInputMethodEvent *event);

    // 设置场景指针
    void setScene(DocumentScene *scene);

signals:
    // 选择发生变化时发出信号，用于更新选择显示
    void selectionNeedsUpdate();

private:
    Document *m_document;
    Cursor *m_cursor;
    Selection *m_selection;
    FormatController *m_formatController;  // 格式控制器
    DocumentScene *m_scene;
    bool m_isSelecting;  // 是否正在选择文本
    int m_selectionStartBlock;  // 选择起始块索引
    int m_selectionStartOffset;  // 选择起始偏移量
};

} // namespace QtWordEditor

#endif // EDITEVENTHANDLER_H