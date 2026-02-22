#include "editcontrol/handlers/EditEventHandler.h"
#include "core/document/Document.h"
#include "editcontrol/cursor/Cursor.h"
#include "editcontrol/selection/Selection.h"
#include "editcontrol/formatting/FormatController.h"
#include "graphics/scene/DocumentScene.h"
#include "graphics/formula/MathItem.h"
#include "graphics/items/TextBlockItem.h"
// 移除 Logger 头文件，使用 Qt 内置日志函数

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

    // ========== 先检查是否有 TextBlockItem 处于公式编辑模式 ==========
    if (m_scene) {
        QList<QGraphicsItem *> items = m_scene->items();
        for (QGraphicsItem *item : items) {
            TextBlockItem *textBlockItem = dynamic_cast<TextBlockItem *>(item);
            if (textBlockItem && textBlockItem->isInMathEditMode()) {
                qDebug() << "[EditEventHandler] TextBlockItem 处于公式编辑模式，不处理按键";
                // 如果有 TextBlockItem 处于公式编辑模式，不处理，让 TextBlockItem 处理
                return false;
            }
        }
    }

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

    qDebug() << "[EditEventHandler::handleMousePress] at:" << scenePos;

    // ========== 先检查是否点击了 MathItem ==========
    QList<QGraphicsItem *> items = m_scene->items(scenePos);
    
    // 先检查所有 items，看是否有 MathItem
    for (QGraphicsItem *item : items) {
        MathItem *mathItem = dynamic_cast<MathItem *>(item);
        if (mathItem) {
            qDebug() << "[EditEventHandler] 点击了 MathItem，不处理选择";
            // 如果点击了 MathItem，直接返回，不处理选择
            m_isSelecting = false;
            return true;
        }
    }
    
    // ========== 关键修复：遍历所有 TextBlockItem，确保它们都退出公式编辑模式 ==========
    bool hadMathEditMode = false;
    QList<QGraphicsItem *> allItems = m_scene->items();
    for (QGraphicsItem *item : allItems) {
        TextBlockItem *textBlockItem = dynamic_cast<TextBlockItem *>(item);
        if (textBlockItem && textBlockItem->isInMathEditMode()) {
            qDebug() << "[EditEventHandler] 发现有 TextBlockItem 处于公式编辑模式，强制退出";
            textBlockItem->exitMathEditMode();
            hadMathEditMode = true;
        }
    }

    // ========== 如果没有点击 MathItem，正常处理选择 ==========
    qDebug() << "[EditEventHandler] 没有点击 MathItem，正常处理选择";
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

    // 获取光标位置
    CursorPosition cursorPos = m_scene->cursorPositionAt(scenePos);

    // 更新选择范围
    m_selection->setRange(
        m_selectionStartBlock,
        m_selectionStartOffset,
        cursorPos.blockIndex,
        cursorPos.offset
    );

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

    qDebug() << QString("EditEventHandler::handleMouseRelease at: (%1, %2)").arg(scenePos.x()).arg(scenePos.y());

    // 获取最终的选择范围
    SelectionRange range = m_selection->range();
    qDebug() << QString("Start: 块%1，偏移%2  End: 块%3，偏移%4 ").arg(range.startBlock).arg(range.startOffset).arg(range.endBlock).arg(range.endOffset);

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