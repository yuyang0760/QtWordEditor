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
    
    // ========== 添加调试输出 ==========
    qDebug() << "FormatController::applyCharacterStyle - 开始应用样式";
    qDebug() << "  范围: 块" << range.startBlock << "偏移" << range.startOffset 
             << "到块" << range.endBlock << "偏移" << range.endOffset;
    qDebug() << "  要应用的样式:";
    qDebug() << "    加粗:" << style.bold();
    qDebug() << "    斜体:" << style.italic();
    qDebug() << "    下划线:" << style.underline();
    
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
        
        qDebug() << "  处理块" << blockIndex << ": 偏移" << startOffset << "到" << endOffset;
        
        // 对于选择范围内的每个位置，获取原样式并合并
        // 这里我们不再只获取起始位置，而是让 ParagraphBlock::setStyle 自己处理
        
        // 创建并推入命令
        SetCharacterStyleCommand *cmd = new SetCharacterStyleCommand(
            m_document, blockIndex, startOffset, endOffset, style);
        m_document->undoStack()->push(cmd);
    }
    
    qDebug() << "FormatController::applyCharacterStyle - 样式应用完成";
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

    // 判断是否有选区
    bool hasSelection = (m_selection && !m_selection->isEmpty());
    if (hasSelection) {
        // ========== 有选区的情况 ==========
        qDebug() << "FormatController::getCurrentDisplayStyle - 有选区";
        SelectionRange range = m_selection->range();
        qDebug() << "  选区: Anchor(" << range.anchorBlock << "," << range.anchorOffset << ")" 
                 << ", Focus(" << range.focusBlock << "," << range.focusOffset << ")";
        
        // 检查选区是否在单个块内
        if (range.startBlock >= 0 && range.startBlock == range.endBlock) {
            Block *block = m_document->block(range.startBlock);
            ParagraphBlock *paraBlock = qobject_cast<ParagraphBlock*>(block);
            if (paraBlock) {
                // ========== 查找选区完全在哪个 Span 内 ==========
                int spanIndex = -1;
                int currentOffset = 0;
                for (int i = 0; i < paraBlock->spanCount(); ++i) {
                    const Span &span = paraBlock->span(i);
                    int spanStart = currentOffset;
                    int spanEnd = spanStart + span.text().length();
                    
                    // 检查 span 是否完全包含选区
                    if (spanStart <= range.startOffset && spanEnd >= range.endOffset) {
                        spanIndex = i;
                        qDebug() << "  选区完全在 Span " << i << " 内: [" << spanStart << "," << spanEnd << "] "
                                 << (span.style().bold() ? "[加粗]" : "[正常]");
                        result = span.style();
                        qDebug() << "  获取到该 Span 的样式: 加粗=" << result.bold() 
                                 << ", 斜体=" << result.italic() 
                                 << ", 下划线=" << result.underline();
                        return result;
                    }
                    
                    currentOffset = spanEnd;
                }
                
                // ========== 如果选区跨多个 Span，显示最后一个选中字符的样式 ==========
                qDebug() << "  选区跨多个 Span，使用最后一个选中字符的样式";
                CursorPosition targetPos = m_selection->focusPosition();
                int targetBlock = targetPos.blockIndex;
                int targetOffset = targetPos.offset - 1;
                
                qDebug() << "  计算目标位置: 块 " << targetBlock << "，偏移 " << targetOffset << " (原 offset=" << targetPos.offset << "-1)";
                
                // 处理边界情况
                if (targetOffset < 0) {
                    if (targetBlock > 0) {
                        targetBlock = targetBlock - 1;
                        Block *prevBlock = m_document->block(targetBlock);
                        if (prevBlock) {
                            ParagraphBlock *prevParaBlock = qobject_cast<ParagraphBlock*>(prevBlock);
                            if (prevParaBlock) {
                                targetOffset = prevParaBlock->length() - 1;
                                qDebug() << "  调整目标位置到前一个块: 块 " << targetBlock << "，偏移 " << targetOffset;
                            }
                        }
                    } else {
                        qDebug() << "  已在文档开头，返回默认样式";
                        return result;
                    }
                }
                
                if (targetOffset >= 0) {
                    result = getStyleAtPosition(targetBlock, targetOffset);
                    qDebug() << "  获取到的样式: 加粗=" << result.bold() 
                             << ", 斜体=" << result.italic() 
                             << ", 下划线=" << result.underline();
                }
                return result;
            }
        }
    } else {
        // ========== 无选区的情况：使用光标前一个字符的样式 ==========
        if (m_cursor) {
            CursorPosition targetPos = m_cursor->position();
            qDebug() << "FormatController::getCurrentDisplayStyle - 无选区，光标位置: (" << targetPos.blockIndex << "," << targetPos.offset << ")";
            
            int targetBlock = targetPos.blockIndex;
            int targetOffset = targetPos.offset - 1;
            
            qDebug() << "  计算目标位置: 块 " << targetBlock << "，偏移 " << targetOffset << " (原 offset=" << targetPos.offset << "-1)";
            
            if (targetOffset >= 0) {
                result = getStyleAtPosition(targetBlock, targetOffset);
                qDebug() << "  获取到的样式: 加粗=" << result.bold() 
                         << ", 斜体=" << result.italic() 
                         << ", 下划线=" << result.underline();
            } else if (targetBlock > 0) {
                targetBlock = targetBlock - 1;
                Block *prevBlock = m_document->block(targetBlock);
                if (prevBlock) {
                    ParagraphBlock *prevParaBlock = qobject_cast<ParagraphBlock*>(prevBlock);
                    if (prevParaBlock) {
                        targetOffset = prevParaBlock->length() - 1;
                        result = getStyleAtPosition(targetBlock, targetOffset);
                        qDebug() << "  调整目标位置到前一个块，获取到的样式: 加粗=" << result.bold();
                    }
                }
            }
        } else {
            qWarning() << "FormatController::getCurrentDisplayStyle(): 无选区且无 Cursor 对象";
        }
    }

    return result;
}

FormatController::StyleConsistency FormatController::getSelectionStyleConsistency() const
{
    StyleConsistency consistency;
    
    if (!m_selection || m_selection->isEmpty()) {
        return consistency; // 无选区时所有属性都一致
    }
    
    SelectionRange range = m_selection->range();
    
    // 检查是否跨多个块
    if (range.startBlock != range.endBlock) {
        qDebug() << "FormatController::getSelectionStyleConsistency - 跨多个块，所有属性都不一致";
        consistency.fontFamilyConsistent = false;
        consistency.fontSizeConsistent = false;
        consistency.boldConsistent = false;
        consistency.italicConsistent = false;
        consistency.underlineConsistent = false;
        return consistency;
    }
    
    // 检查是否在单个块内
    if (range.startBlock < 0) {
        return consistency;
    }
    
    Block *block = m_document->block(range.startBlock);
    ParagraphBlock *paraBlock = qobject_cast<ParagraphBlock*>(block);
    if (!paraBlock) {
        return consistency;
    }
    
    // 收集所有与选区重叠的 Span
    QList<CharacterStyle> selectedStyles;
    int currentOffset = 0;
    for (int i = 0; i < paraBlock->spanCount(); ++i) {
        const Span &span = paraBlock->span(i);
        int spanStart = currentOffset;
        int spanEnd = spanStart + span.text().length();
        
        // 检查 span 是否与选区重叠
        if (!(spanEnd <= range.startOffset || spanStart >= range.endOffset)) {
            selectedStyles.append(span.style());
            qDebug() << "  包含 Span " << i << ": 加粗=" << span.style().bold() 
                     << ", 斜体=" << span.style().italic()
                     << ", 下划线=" << span.style().underline()
                     << ", 字体=" << span.style().fontFamily()
                     << ", 字号=" << span.style().fontSize();
        }
        
        currentOffset = spanEnd;
    }
    
    if (selectedStyles.size() <= 1) {
        // 只有一个 Span，所有属性都一致
        qDebug() << "FormatController::getSelectionStyleConsistency - 只有 " << selectedStyles.size() << " 个 Span，所有属性都一致";
        return consistency;
    }
    
    // 比较所有 Span 的各个属性
    CharacterStyle firstStyle = selectedStyles[0];
    
    for (int i = 1; i < selectedStyles.size(); ++i) {
        CharacterStyle currentStyle = selectedStyles[i];
        
        // 检查字体
        if (consistency.fontFamilyConsistent && currentStyle.fontFamily() != firstStyle.fontFamily()) {
            consistency.fontFamilyConsistent = false;
            qDebug() << "  字体不一致：" << firstStyle.fontFamily() << " vs " << currentStyle.fontFamily();
        }
        
        // 检查字号
        if (consistency.fontSizeConsistent && currentStyle.fontSize() != firstStyle.fontSize()) {
            consistency.fontSizeConsistent = false;
            qDebug() << "  字号不一致：" << firstStyle.fontSize() << " vs " << currentStyle.fontSize();
        }
        
        // 检查粗体
        if (consistency.boldConsistent && currentStyle.bold() != firstStyle.bold()) {
            consistency.boldConsistent = false;
            qDebug() << "  粗体不一致：" << firstStyle.bold() << " vs " << currentStyle.bold();
        }
        
        // 检查斜体
        if (consistency.italicConsistent && currentStyle.italic() != firstStyle.italic()) {
            consistency.italicConsistent = false;
            qDebug() << "  斜体不一致：" << firstStyle.italic() << " vs " << currentStyle.italic();
        }
        
        // 检查下划线
        if (consistency.underlineConsistent && currentStyle.underline() != firstStyle.underline()) {
            consistency.underlineConsistent = false;
            qDebug() << "  下划线不一致：" << firstStyle.underline() << " vs " << currentStyle.underline();
        }
    }
    
    qDebug() << "FormatController::getSelectionStyleConsistency - 一致性检查结果："
             << "字体=" << consistency.fontFamilyConsistent
             << ", 字号=" << consistency.fontSizeConsistent
             << ", 粗体=" << consistency.boldConsistent
             << ", 斜体=" << consistency.italicConsistent
             << ", 下划线=" << consistency.underlineConsistent;
    
    return consistency;
}

bool FormatController::isSelectionStyleConsistent() const
{
    StyleConsistency consistency = getSelectionStyleConsistency();
    return consistency.fontFamilyConsistent && 
           consistency.fontSizeConsistent && 
           consistency.boldConsistent && 
           consistency.italicConsistent && 
           consistency.underlineConsistent;
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