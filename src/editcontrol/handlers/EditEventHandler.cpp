#include "editcontrol/handlers/EditEventHandler.h"
#include "core/document/Document.h"
#include "editcontrol/cursor/UnifiedCursor.h"
#include "editcontrol/cursor/CoordinatePath.h"
#include "editcontrol/cursor/PathSegment.h"
#include "editcontrol/selection/Selection.h"
#include "editcontrol/formatting/FormatController.h"
#include "graphics/scene/DocumentScene.h"
#include "graphics/formula/MathItem.h"
#include "graphics/formula/GenericMathItem.h"
#include "graphics/items/TextBlockItem.h"
// 移除 Logger 头文件，使用 Qt 内置日志函数

namespace QtWordEditor {

EditEventHandler::EditEventHandler(Document *document, UnifiedCursor *cursor, Selection *selection,
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
            if (m_cursor->unifiedPosition().isMathMode()) {
                m_cursor->mathMoveLeft();
            } else {
                m_cursor->moveLeft();
            }
        }
        handled = true;
        break;
    case Qt::Key_Right:
        if (event->modifiers() & Qt::ShiftModifier) {
            // Extend selection right
        } else {
            if (m_cursor->unifiedPosition().isMathMode()) {
                m_cursor->mathMoveRight();
            } else {
                m_cursor->moveRight();
            }
        }
        handled = true;
        break;
    case Qt::Key_Up:
        if (m_cursor->unifiedPosition().isMathMode()) {
            m_cursor->mathMoveUp();
        } else {
            m_cursor->moveUp();
        }
        handled = true;
        break;
    case Qt::Key_Down:
        if (m_cursor->unifiedPosition().isMathMode()) {
            m_cursor->mathMoveDown();
        } else {
            m_cursor->moveDown();
        }
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
    case Qt::Key_Escape:
        // 退出公式模式
        m_cursor->exitMathMode();
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

    // ========== 检查是否点击了 MathItem ==========
    bool clickedMathItem = false;
    UnifiedCursorPosition unifiedPos;
    unifiedPos.mathPath = std::nullopt;
    
    QList<QGraphicsItem *> itemsAtPos = m_scene->items(scenePos);
    qDebug() << "[EditEventHandler] itemsAtPos 数量:" << itemsAtPos.size();
    
    // 打印所有 itemsAtPos 的信息，看看都是什么
    for (int i = 0; i < itemsAtPos.size(); ++i) {
        QGraphicsItem *item = itemsAtPos.at(i);
        qDebug() << "  Item" << i << ":" << item 
                 << " type:" << item->type() 
                 << " pos:" << item->pos();
    }
    
    // 找到点击位置的根 MathItem（最顶层的 MathItem，排除 TextBlockItem）
    MathItem *rootMathItem = nullptr;
    for (QGraphicsItem *item : itemsAtPos) {
        MathItem *mathItem = dynamic_cast<MathItem *>(item);
        if (mathItem) {
            qDebug() << "[EditEventHandler] 找到 MathItem:" << mathItem << " type:" << mathItem->type();
            
            // 检查这是不是根 MathItem（没有父 MathItem）
            if (!mathItem->parentMathItem()) {
                rootMathItem = mathItem;
                qDebug() << "[EditEventHandler] 找到根 MathItem:" << rootMathItem;
                break;
            }
            
            // 如果不是根，查找父元素直到根
            MathItem *parent = mathItem->parentMathItem();
            while (parent) {
                if (!parent->parentMathItem()) {
                    rootMathItem = parent;
                    qDebug() << "[EditEventHandler] 从父元素找到根 MathItem:" << rootMathItem;
                    break;
                }
                parent = parent->parentMathItem();
            }
        }
    }
    
    if (rootMathItem) {
        clickedMathItem = true;
        
        // 先获取基础的光标位置
        CursorPosition cursorPos = m_scene->cursorPositionAt(scenePos);
        unifiedPos.blockIndex = cursorPos.blockIndex;
        unifiedPos.offset = cursorPos.offset;
        
        // 现在从根 MathItem 开始，向下构建坐标路径
        CoordinatePath mathPath;
        MathItem *currentContainer = rootMathItem;
        QPointF currentLocalPos = rootMathItem->mapFromScene(scenePos);
        
        while (currentContainer) {
            qDebug() << "[EditEventHandler] 当前容器:" << currentContainer 
                     << " type:" << currentContainer->type()
                     << " localPos:" << currentLocalPos;
            
            // 调用 hitTestRegion 判断点击的是哪个子区域
            int childIndex = currentContainer->hitTestRegion(currentLocalPos);
            qDebug() << "[EditEventHandler] hitTestRegion 返回:" << childIndex;
            
            if (childIndex >= 0) {
                // 添加路径段
                PathSegment segment(currentContainer, childIndex, 0);
                mathPath.push(segment);
                qDebug() << "[EditEventHandler] 添加路径段，容器:" << currentContainer 
                         << " childIndex:" << childIndex;
                
                // 获取子元素
                MathItem *childItem = currentContainer->childAt(childIndex);
                if (childItem) {
                    // 转换坐标到子元素的局部坐标
                    currentLocalPos = childItem->mapFromItem(currentContainer, currentLocalPos);
                    currentContainer = childItem;
                } else {
                    // 没有子元素了，停止
                    currentContainer = nullptr;
                }
            } else {
                // 没有找到子区域，停止
                currentContainer = nullptr;
            }
        }
        
        // 只要有根 MathItem，就设置 mathPath（即使是空的，也表示在公式内）
        unifiedPos.mathPath = mathPath;
        qDebug() << "[EditEventHandler] 设置了 mathPath，深度:" << mathPath.depth();
        
        // 如果根 MathItem 是 GenericMathItem 且深度为0，尝试计算点击位置的文本偏移
        if (mathPath.depth() == 0) {
            GenericMathItem *genericItem = dynamic_cast<GenericMathItem*>(rootMathItem);
            if (genericItem) {
                QPointF localPos = genericItem->mapFromScene(scenePos);
                int textOffset = genericItem->hitTest(localPos);
                qDebug() << "[EditEventHandler] GenericMathItem 的 hitTest 返回文本偏移:" << textOffset;
                unifiedPos.mathTextOffset = textOffset;
            }
        }
    }

    // ========== 设置光标位置 ==========
    if (clickedMathItem && unifiedPos.mathPath.has_value()) {
        // 使用新的统一光标位置
        qDebug() << "[EditEventHandler] 准备调用 setUnifiedPosition, 当前 cursor 位置:" << m_cursor->unifiedPosition().isMathMode();
        m_cursor->setUnifiedPosition(unifiedPos);
        qDebug() << "[EditEventHandler] 设置了带坐标路径的光标位置, 新位置:" << m_cursor->unifiedPosition().isMathMode();
    } else {
        // 使用旧的光标位置
        CursorPosition cursorPos = m_scene->cursorPositionAt(scenePos);
        m_cursor->setPosition(cursorPos);
        qDebug() << "[EditEventHandler] 设置了普通光标位置";
    }

    // 开始选择
    m_isSelecting = true;
    CursorPosition cursorPosForSelection = m_cursor->position();
    m_selectionStartBlock = cursorPosForSelection.blockIndex;
    m_selectionStartOffset = cursorPosForSelection.offset;

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

    // 先检查是否有 TextBlockItem 处于公式编辑模式
    if (m_scene) {
        QList<QGraphicsItem *> items = m_scene->items();
        for (QGraphicsItem *item : items) {
            TextBlockItem *textBlockItem = dynamic_cast<TextBlockItem *>(item);
            if (textBlockItem && textBlockItem->isInMathEditMode()) {
                qDebug() << "[EditEventHandler] TextBlockItem 处于公式编辑模式，不处理输入法事件";
                // 如果有 TextBlockItem 处于公式编辑模式，不处理，让 TextBlockItem 处理
                return false;
            }
        }
    }

    qDebug() << "EditEventHandler::handleInputMethod called";
    qDebug() << "  commitString:" << event->commitString();
    qDebug() << "  preeditString:" << event->preeditString();
    
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