#include "editcontrol/formatting/FormatController.h"
#include "core/document/Document.h"
#include "core/document/ParagraphBlock.h"
#include "core/document/Section.h"
#include "editcontrol/selection/Selection.h"
#include "editcontrol/cursor/Cursor.h"
#include "core/commands/SetCharacterStyleCommand.h"
#include "core/commands/SetParagraphStyleCommand.h"
#include "core/styles/StyleManager.h"
#include <QDebug>

namespace QtWordEditor {

FormatController::FormatController(Document *document, Cursor *cursor,
                                    Selection *selection,
                                    StyleManager *styleManager, QObject *parent)
    : QObject(parent)
    , m_document(document)
    , m_cursor(cursor)
    , m_selection(selection)
    , m_styleManager(styleManager)
    , m_isInputStyleOverridden(false)
{
}

FormatController::~FormatController()
{
}

void FormatController::applyCharacterStyle(const CharacterStyle &style)
{
    if (!m_document || !m_selection)
        return;
    
    SelectionRange range = m_selection->range();
    if (range.isEmpty())
        return;
    
    range.normalize();
    
    // 遍历选择范围内的每个块
    for (int blockIndex = range.startBlock; blockIndex <= range.endBlock; ++blockIndex) {
        Block *block = m_document->block(blockIndex);
        if (!block)
            continue;
        
        ParagraphBlock *paraBlock = qobject_cast<ParagraphBlock*>(block);
        if (!paraBlock)
            continue;
        
        // 计算当前块的起始和结束偏移量
        int startOffset = (blockIndex == range.startBlock) ? range.startOffset : 0;
        int endOffset = (blockIndex == range.endBlock) ? range.endOffset : paraBlock->length();
        
        if (startOffset >= endOffset)
            continue;
        
        // 对于选择范围内的每个位置，获取原样式并合并
        // 这里我们创建一个合并后的样式，直接应用到整个范围
        CharacterStyle mergedStyle;
        
        // 获取起始位置的当前样式作为基础
        if (startOffset < paraBlock->length()) {
            CharacterStyle currentStyle = paraBlock->styleAt(startOffset);
            // 使用 mergeWith 合并，只覆盖显式设置的属性
            mergedStyle = currentStyle.mergeWith(style);
        } else {
            mergedStyle = style;
        }
        
        // 创建并推入命令
        SetCharacterStyleCommand *cmd = new SetCharacterStyleCommand(
            m_document, blockIndex, startOffset, endOffset, mergedStyle);
        m_document->undoStack()->push(cmd);
    }
}

void FormatController::applyNamedCharacterStyle(const QString &styleName)
{
    if (!m_styleManager || !m_styleManager->hasCharacterStyle(styleName))
        return;
    
    // 获取解析继承后的样式
    CharacterStyle style = m_styleManager->getResolvedCharacterStyle(styleName);
    applyCharacterStyle(style);
}

void FormatController::setFont(const QFont &font)
{
    CharacterStyle style;
    style.setFont(font);
    applyCharacterStyle(style);
}

void FormatController::setFontSize(int size)
{
    CharacterStyle style;
    style.setFontSize(size);
    applyCharacterStyle(style);
}

void FormatController::setBold(bool bold)
{
    CharacterStyle style;
    style.setBold(bold);
    applyCharacterStyle(style);
}

void FormatController::setItalic(bool italic)
{
    CharacterStyle style;
    style.setItalic(italic);
    applyCharacterStyle(style);
}

void FormatController::setUnderline(bool underline)
{
    CharacterStyle style;
    style.setUnderline(underline);
    applyCharacterStyle(style);
}

void FormatController::setTextColor(const QColor &color)
{
    CharacterStyle style;
    style.setTextColor(color);
    applyCharacterStyle(style);
}

void FormatController::setBackgroundColor(const QColor &color)
{
    CharacterStyle style;
    style.setBackgroundColor(color);
    applyCharacterStyle(style);
}

CharacterStyle FormatController::getCurrentCharacterStyle() const
{
    CharacterStyle result;
    
    if (!m_document || !m_selection)
        return result;
    
    SelectionRange range = m_selection->range();
    if (range.isEmpty())
        return result;
    
    range.normalize();
    
    // 获取起始位置的样式
    Block *block = m_document->block(range.startBlock);
    if (!block)
        return result;
    
    ParagraphBlock *paraBlock = qobject_cast<ParagraphBlock*>(block);
    if (!paraBlock)
        return result;
    
    if (range.startOffset < paraBlock->length()) {
        result = paraBlock->styleAt(range.startOffset);
    }
    
    return result;
}

void FormatController::applyParagraphStyle(const ParagraphStyle &style)
{
    if (!m_document || !m_selection)
        return;
    
    SelectionRange range = m_selection->range();
    if (range.isEmpty())
        return;
    
    range.normalize();
    
    QList<int> blockIndices;
    
    // 收集选择范围内的所有块索引
    for (int blockIndex = range.startBlock; blockIndex <= range.endBlock; ++blockIndex) {
        Block *block = m_document->block(blockIndex);
        if (!block)
            continue;
        
        ParagraphBlock *paraBlock = qobject_cast<ParagraphBlock*>(block);
        if (!paraBlock)
            continue;
        
        blockIndices.append(blockIndex);
    }
    
    if (blockIndices.isEmpty())
        return;
    
    // 对于每个块，获取原样式并合并，然后创建命令
    // 注意：SetParagraphStyleCommand 需要处理合并逻辑
    // 这里我们先简化处理，直接应用样式
    SetParagraphStyleCommand *cmd = new SetParagraphStyleCommand(
        m_document, blockIndices, style);
    m_document->undoStack()->push(cmd);
}

void FormatController::applyNamedParagraphStyle(const QString &styleName)
{
    if (!m_styleManager || !m_styleManager->hasParagraphStyle(styleName))
        return;
    
    // 获取解析继承后的样式
    ParagraphStyle style = m_styleManager->getResolvedParagraphStyle(styleName);
    applyParagraphStyle(style);
}

void FormatController::setAlignment(QtWordEditor::ParagraphAlignment align)
{
    ParagraphStyle style;
    style.setAlignment(align);
    applyParagraphStyle(style);
}

void FormatController::setLeftIndent(qreal indent)
{
    ParagraphStyle style;
    style.setLeftIndent(indent);
    applyParagraphStyle(style);
}

void FormatController::setRightIndent(qreal indent)
{
    ParagraphStyle style;
    style.setRightIndent(indent);
    applyParagraphStyle(style);
}

void FormatController::setFirstLineIndent(qreal indent)
{
    ParagraphStyle style;
    style.setFirstLineIndent(indent);
    applyParagraphStyle(style);
}

void FormatController::setLineHeight(int percent)
{
    ParagraphStyle style;
    style.setLineHeight(percent);
    applyParagraphStyle(style);
}

void FormatController::setSpaceBefore(qreal space)
{
    ParagraphStyle style;
    style.setSpaceBefore(space);
    applyParagraphStyle(style);
}

void FormatController::setSpaceAfter(qreal space)
{
    ParagraphStyle style;
    style.setSpaceAfter(space);
    applyParagraphStyle(style);
}

ParagraphStyle FormatController::getCurrentParagraphStyle() const
{
    ParagraphStyle result;
    
    if (!m_document || !m_selection)
        return result;
    
    SelectionRange range = m_selection->range();
    if (range.isEmpty())
        return result;
    
    // 获取起始块的段落样式
    Block *block = m_document->block(range.startBlock);
    if (!block)
        return result;
    
    ParagraphBlock *paraBlock = qobject_cast<ParagraphBlock*>(block);
    if (!paraBlock)
        return result;
    
    result = paraBlock->paragraphStyle();
    
    return result;
}

CharacterStyle FormatController::getStyleAtPosition(int blockIndex, int offset) const
{
    CharacterStyle result;

    if (!m_document)
        return result;

    // 获取指定块
    Block *block = m_document->block(blockIndex);
    if (!block)
        return result;

    ParagraphBlock *paraBlock = qobject_cast<ParagraphBlock*>(block);
    if (!paraBlock)
        return result;

    // 验证偏移量的有效性
    if (offset >= 0 && offset < paraBlock->length()) {
        result = paraBlock->styleAt(offset);
    }

    return result;
}

CursorPosition FormatController::getSelectionEndPosition() const
{
    if (!m_selection)
        return {-1, 0};

    // 直接从 Selection 获取焦点位置（Focus）
    return m_selection->focusPosition();
}

CharacterStyle FormatController::getCurrentDisplayStyle() const
{
    CharacterStyle result;

    if (!m_document)
        return result;

    CursorPosition targetPos;

    // 判断是否有选区
    if (m_selection && !m_selection->isEmpty()) {
        // 有选区：获取选区终点（Focus）位置
        targetPos = m_selection->focusPosition();
    } else {
        // 无选区：获取光标位置
        if (m_cursor) {
            targetPos = m_cursor->position();
        } else {
            qWarning() << "FormatController::getCurrentDisplayStyle(): 无选区且无 Cursor 对象，无法获取光标位置";
            return result;
        }
    }

    // 计算目标位置：终点位置 - 1
    int targetBlock = targetPos.blockIndex;
    int targetOffset = targetPos.offset - 1;

    // 处理边界情况：如果偏移量为 -1（即终点在块的开头）
    if (targetOffset < 0) {
        // 尝试找到前一个块
        if (targetBlock > 0) {
            targetBlock = targetBlock - 1;
            Block *prevBlock = m_document->block(targetBlock);
            if (prevBlock) {
                ParagraphBlock *prevParaBlock = qobject_cast<ParagraphBlock*>(prevBlock);
                if (prevParaBlock) {
                    targetOffset = prevParaBlock->length() - 1;
                }
            }
        } else {
            // 已经在文档开头，没有前一个字符，返回空样式
            return result;
        }
    }

    // 获取目标位置的样式
    if (targetOffset >= 0) {
        result = getStyleAtPosition(targetBlock, targetOffset);
    }

    return result;
}

CharacterStyle FormatController::getCurrentInputStyle() const
{
    if (m_isInputStyleOverridden) {
        // 覆盖模式：返回用户设置的样式
        return m_currentInputStyle;
    } else {
        // 继承模式：返回光标前一个字符的样式
        return getCurrentDisplayStyle();
    }
}

void FormatController::setCurrentInputStyle(const CharacterStyle &style)
{
    // 用户通过工具栏设置样式，进入覆盖模式
    m_isInputStyleOverridden = true;
    
    // 合并样式：如果用户只设置了部分属性（如只设置粗体），
    // 则保留其他属性的当前值
    if (m_isInputStyleOverridden) {
        m_currentInputStyle = m_currentInputStyle.mergeWith(style);
    } else {
        m_currentInputStyle = getCurrentDisplayStyle().mergeWith(style);
    }
}

void FormatController::onCursorMoved()
{
  //  QDebug() << "FormatController::onCursorMoved() called";
    
    // 光标移动时，退出覆盖模式，回到继承模式
    m_isInputStyleOverridden = false;
    
    // 更新当前输入样式为光标前一个字符的样式
    m_currentInputStyle = getCurrentDisplayStyle();
    
  //  QDebug() << "  isInputStyleOverridden reset to false, currentInputStyle updated";
}

} // namespace QtWordEditor