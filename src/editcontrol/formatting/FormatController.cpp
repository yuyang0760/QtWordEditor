#include "editcontrol/formatting/FormatController.h"
#include "core/document/Document.h"
#include "core/document/ParagraphBlock.h"
#include "core/document/Section.h"
#include "editcontrol/selection/Selection.h"
#include "editcontrol/cursor/Cursor.h"
#include "core/commands/SetCharacterStyleCommand.h"
#include "core/commands/SetParagraphStyleCommand.h"
#include "core/styles/StyleManager.h"
#include "core/utils/Logger.h"

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
    
    LOG_DEBUG("FormatController::applyCharacterStyle - 开始应用样式");
    LOG_DEBUG(QString("  范围: 块%1 偏移%2 到块%3 偏移%4")
        .arg(range.startBlock).arg(range.startOffset)
        .arg(range.endBlock).arg(range.endOffset));
    LOG_DEBUG("  要应用的样式:");
    LOG_DEBUG(QString("    加粗: %1").arg(style.bold()));
    LOG_DEBUG(QString("    斜体: %1").arg(style.italic()));
    LOG_DEBUG(QString("    下划线: %1").arg(style.underline()));
    
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
        
        LOG_DEBUG(QString("  处理块%1: 偏移%2 到%3")
            .arg(blockIndex).arg(startOffset).arg(endOffset));
        
        // 对于选择范围内的每个位置，获取原样式并合并
        // 这里我们不再只获取起始位置，而是让 ParagraphBlock::setStyle 自己处理
        
        // 创建并推入命令
        SetCharacterStyleCommand *cmd = new SetCharacterStyleCommand(
            m_document, blockIndex, startOffset, endOffset, style);
        m_document->undoStack()->push(cmd);
    }
    
    LOG_DEBUG("FormatController::applyCharacterStyle - 样式应用完成");
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
    LOG_DEBUG("FormatController::setFont - 被调用，字体: " + font.family());
    applySingleProperty([&font](CharacterStyle& style) {
        style.setFont(font);
    });
    LOG_DEBUG("  setFont 完成");
}

void FormatController::setFontFamily(const QString &family)
{
    LOG_DEBUG("FormatController::setFontFamily - 被调用，字体族: " + family);
    applySingleProperty([&family](CharacterStyle& style) {
        style.setFontFamily(family);
    });
    LOG_DEBUG("  setFontFamily 完成");
}

void FormatController::setFontSize(int size)
{
    LOG_DEBUG(QString("FormatController::setFontSize - 被调用，字号: %1").arg(size));
    applySingleProperty([size](CharacterStyle& style) {
        style.setFontSize(size);
    });
    LOG_DEBUG("  setFontSize 完成");
}

void FormatController::setBold(bool bold)
{
    applySingleProperty([bold](CharacterStyle& style) {
        style.setBold(bold);
    });
}

void FormatController::setItalic(bool italic)
{
    applySingleProperty([italic](CharacterStyle& style) {
        style.setItalic(italic);
    });
}

void FormatController::setUnderline(bool underline)
{
    applySingleProperty([underline](CharacterStyle& style) {
        style.setUnderline(underline);
    });
}

void FormatController::setTextColor(const QColor &color)
{
    applySingleProperty([&color](CharacterStyle& style) {
        style.setTextColor(color);
    });
}

void FormatController::setBackgroundColor(const QColor &color)
{
    applySingleProperty([&color](CharacterStyle& style) {
        style.setBackgroundColor(color);
    });
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
    applySingleParagraphProperty([align](ParagraphStyle& style) {
        style.setAlignment(align);
    });
}

void FormatController::setLeftIndent(qreal indent)
{
    applySingleParagraphProperty([indent](ParagraphStyle& style) {
        style.setLeftIndent(indent);
    });
}

void FormatController::setRightIndent(qreal indent)
{
    applySingleParagraphProperty([indent](ParagraphStyle& style) {
        style.setRightIndent(indent);
    });
}

void FormatController::setFirstLineIndent(qreal indent)
{
    applySingleParagraphProperty([indent](ParagraphStyle& style) {
        style.setFirstLineIndent(indent);
    });
}

void FormatController::setLineHeight(int percent)
{
    applySingleParagraphProperty([percent](ParagraphStyle& style) {
        style.setLineHeight(percent);
    });
}

void FormatController::setSpaceBefore(qreal space)
{
    applySingleParagraphProperty([space](ParagraphStyle& style) {
        style.setSpaceBefore(space);
    });
}

void FormatController::setSpaceAfter(qreal space)
{
    applySingleParagraphProperty([space](ParagraphStyle& style) {
        style.setSpaceAfter(space);
    });
}

void FormatController::setHangingIndent(qreal indent)
{
    // 悬挂缩进的实现原理：
    // 1. 设置左缩进为悬挂缩进值
    // 2. 设置首行缩进为负值（-悬挂缩进值）
    // 这样第一行正常显示，其他行都向右缩进
    ParagraphStyle style;
    style.setLeftIndent(indent);
    style.setFirstLineIndent(-indent);
    applyParagraphStyle(style);
}

void FormatController::clearHangingIndent()
{
    // 清除悬挂缩进：
    // 1. 左缩进恢复为 0
    // 2. 首行缩进恢复为默认值
    ParagraphStyle style;
    style.setLeftIndent(0.0);
    style.setFirstLineIndent(Constants::DEFAULT_FIRST_LINE_INDENT);
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
        LOG_DEBUG("FormatController::getCurrentDisplayStyle - 有选区");
        SelectionRange range = m_selection->range();
        LOG_DEBUG(QString("  选区: Anchor(%1, %2), Focus(%3, %4)")
            .arg(range.anchorBlock).arg(range.anchorOffset)
            .arg(range.focusBlock).arg(range.focusOffset));
        
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
                        LOG_DEBUG(QString("  选区完全在 Span %1 内: [%2, %3] %4")
                            .arg(i).arg(spanStart).arg(spanEnd)
                            .arg(span.style().bold() ? "[加粗]" : "[正常]"));
                        result = span.style();
                        LOG_DEBUG(QString("  获取到该 Span 的样式: 加粗=%1, 斜体=%2, 下划线=%3")
                            .arg(result.bold()).arg(result.italic()).arg(result.underline()));
                        return result;
                    }
                    
                    currentOffset = spanEnd;
                }
                
                // ========== 如果选区跨多个 Span，返回默认空样式 ==========
                LOG_DEBUG("  选区跨多个 Span，返回默认空样式");
                return result;
            }
        }
    } else {
        // ========== 无选区的情况：使用光标前一个字符的样式 ==========
        if (m_cursor) {
            CursorPosition targetPos = m_cursor->position();
            LOG_DEBUG(QString("FormatController::getCurrentDisplayStyle - 无选区，光标位置: (%1, %2)")
                .arg(targetPos.blockIndex).arg(targetPos.offset));
            
            int targetBlock = targetPos.blockIndex;
            int targetOffset = targetPos.offset - 1;
            
            LOG_DEBUG(QString("  计算目标位置: 块 %1，偏移 %2 (原 offset=%3-1)")
                .arg(targetBlock).arg(targetOffset).arg(targetPos.offset));
            
            if (targetOffset >= 0) {
                result = getStyleAtPosition(targetBlock, targetOffset);
                LOG_DEBUG(QString("  获取到的样式: 加粗=%1, 斜体=%2, 下划线=%3")
                    .arg(result.bold()).arg(result.italic()).arg(result.underline()));
            } else if (targetBlock > 0) {
                targetBlock = targetBlock - 1;
                Block *prevBlock = m_document->block(targetBlock);
                if (prevBlock) {
                    ParagraphBlock *prevParaBlock = qobject_cast<ParagraphBlock*>(prevBlock);
                    if (prevParaBlock) {
                        targetOffset = prevParaBlock->length() - 1;
                        result = getStyleAtPosition(targetBlock, targetOffset);
                        LOG_DEBUG(QString("  调整目标位置到前一个块，获取到的样式: 加粗=%1")
                            .arg(result.bold()));
                    }
                }
            }
        } else {
            LOG_WARNING("FormatController::getCurrentDisplayStyle(): 无选区且无 Cursor 对象");
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
    
    // 使用辅助方法收集样式
    QList<CharacterStyle> selectedStyles = collectSelectionStyles();
    
    if (selectedStyles.isEmpty()) {
        LOG_DEBUG("FormatController::getSelectionStyleConsistency - 没有找到任何 Span");
        return consistency;
    }
    
    // 使用第一个 Span 的样式作为基准
    CharacterStyle firstStyle = selectedStyles[0];
    
    // 初始化一致时的属性值
    consistency.consistentFontFamily = firstStyle.fontFamily();
    consistency.consistentFontSize = firstStyle.fontSize();
    consistency.consistentBold = firstStyle.bold();
    consistency.consistentItalic = firstStyle.italic();
    consistency.consistentUnderline = firstStyle.underline();
    
    if (selectedStyles.size() <= 1) {
        // 只有一个 Span，所有属性都一致
        LOG_DEBUG(QString("FormatController::getSelectionStyleConsistency - 只有 %1 个 Span，所有属性都一致")
            .arg(selectedStyles.size()));
        LOG_DEBUG(QString("  一致的属性值：加粗=%1, 斜体=%2")
            .arg(consistency.consistentBold).arg(consistency.consistentItalic));
        return consistency;
    }
    
    // 比较所有 Span 的各个属性
    for (int i = 1; i < selectedStyles.size(); ++i) {
        CharacterStyle currentStyle = selectedStyles[i];
        
        // 检查字体
        if (consistency.fontFamilyConsistent && currentStyle.fontFamily() != firstStyle.fontFamily()) {
            consistency.fontFamilyConsistent = false;
            LOG_DEBUG(QString("  字体不一致：%1 vs %2")
                .arg(firstStyle.fontFamily()).arg(currentStyle.fontFamily()));
        }
        
        // 检查字号
        if (consistency.fontSizeConsistent && currentStyle.fontSize() != firstStyle.fontSize()) {
            consistency.fontSizeConsistent = false;
            LOG_DEBUG(QString("  字号不一致：%1 vs %2")
                .arg(firstStyle.fontSize()).arg(currentStyle.fontSize()));
        }
        
        // 检查粗体
        if (consistency.boldConsistent && currentStyle.bold() != firstStyle.bold()) {
            consistency.boldConsistent = false;
            LOG_DEBUG(QString("  粗体不一致：%1 vs %2")
                .arg(firstStyle.bold()).arg(currentStyle.bold()));
        }
        
        // 检查斜体
        if (consistency.italicConsistent && currentStyle.italic() != firstStyle.italic()) {
            consistency.italicConsistent = false;
            LOG_DEBUG(QString("  斜体不一致：%1 vs %2")
                .arg(firstStyle.italic()).arg(currentStyle.italic()));
        }
        
        // 检查下划线
        if (consistency.underlineConsistent && currentStyle.underline() != firstStyle.underline()) {
            consistency.underlineConsistent = false;
            LOG_DEBUG(QString("  下划线不一致：%1 vs %2")
                .arg(firstStyle.underline()).arg(currentStyle.underline()));
        }
    }
    
    LOG_DEBUG(QString("FormatController::getSelectionStyleConsistency - 一致性检查结果：字体=%1, 字号=%2, 粗体=%3, 斜体=%4, 下划线=%5")
        .arg(consistency.fontFamilyConsistent)
        .arg(consistency.fontSizeConsistent)
        .arg(consistency.boldConsistent)
        .arg(consistency.italicConsistent)
        .arg(consistency.underlineConsistent));
    LOG_DEBUG(QString("  一致的属性值：加粗=%1, 斜体=%2")
        .arg(consistency.consistentBold).arg(consistency.consistentItalic));
    
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

bool FormatController::isSelectionAllBold() const
{
    bool result = checkSelectionAll([](const CharacterStyle& style) {
        return style.bold();
    });
    LOG_DEBUG(QString("FormatController::isSelectionAllBold - 结果: %1").arg(result));
    return result;
}

bool FormatController::isSelectionAllItalic() const
{
    bool result = checkSelectionAll([](const CharacterStyle& style) {
        return style.italic();
    });
    LOG_DEBUG(QString("FormatController::isSelectionAllItalic - 结果: %1").arg(result));
    return result;
}

bool FormatController::isSelectionAllUnderline() const
{
    bool result = checkSelectionAll([](const CharacterStyle& style) {
        return style.underline();
    });
    LOG_DEBUG(QString("FormatController::isSelectionAllUnderline - 结果: %1").arg(result));
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

// ========== 辅助方法实现 ==========

QList<CharacterStyle> FormatController::collectSelectionStyles() const
{
    QList<CharacterStyle> result;
    
    if (!m_selection || m_selection->isEmpty() || !m_document) {
        return result;
    }
    
    SelectionRange range = m_selection->range();
    
    // 遍历从 startBlock 到 endBlock 的所有块
    for (int blockIndex = range.startBlock; blockIndex <= range.endBlock; ++blockIndex) {
        ParagraphBlock *paraBlock = getParagraphBlock(blockIndex);
        if (!paraBlock) {
            continue;
        }
        
        // 计算当前块的起始和结束偏移量
        int blockStartOffset = (blockIndex == range.startBlock) ? range.startOffset : 0;
        int blockEndOffset = (blockIndex == range.endBlock) ? range.endOffset : paraBlock->length();
        
        LOG_DEBUG(QString("FormatController::collectSelectionStyles - 处理块%1: 偏移%2到%3").arg(blockIndex).arg(blockStartOffset).arg(blockEndOffset));
        
        // 收集当前块中与选区重叠的 Span
        int currentOffset = 0;
        for (int i = 0; i < paraBlock->spanCount(); ++i) {
            const Span &span = paraBlock->span(i);
            int spanStart = currentOffset;
            int spanEnd = spanStart + span.text().length();
            
            // 检查 span 是否与选区重叠
            if (!(spanEnd <= blockStartOffset || spanStart >= blockEndOffset)) {
                result.append(span.style());
                LOG_DEBUG(QString("  包含块%1的 Span %2: 加粗=%3, 斜体=%4, 下划线=%5, 字体=%6, 字号=%7")
                    .arg(blockIndex).arg(i)
                    .arg(span.style().bold()).arg(span.style().italic())
                    .arg(span.style().underline()).arg(span.style().fontFamily())
                    .arg(span.style().fontSize()));
            }
            
            currentOffset = spanEnd;
        }
    }
    
    return result;
}

bool FormatController::checkSelectionAll(const std::function<bool(const CharacterStyle&)>& checkFunc) const
{
    QList<CharacterStyle> styles = collectSelectionStyles();
    if (styles.isEmpty()) {
        return false;
    }
    
    for (const CharacterStyle& style : styles) {
        if (!checkFunc(style)) {
            return false;
        }
    }
    
    return true;
}

void FormatController::applySingleProperty(const std::function<void(CharacterStyle&)>& setPropertyFunc)
{
    CharacterStyle style;
    style.clearAllProperties();
    setPropertyFunc(style);
    applyCharacterStyle(style);
}

ParagraphBlock* FormatController::getParagraphBlock(int blockIndex) const
{
    if (!m_document) {
        return nullptr;
    }
    
    Block *block = m_document->block(blockIndex);
    if (!block) {
        return nullptr;
    }
    
    return qobject_cast<ParagraphBlock*>(block);
}

bool FormatController::validateSelection() const
{
    if (!m_document || !m_selection) {
        return false;
    }
    
    SelectionRange range = m_selection->range();
    if (range.isEmpty()) {
        return false;
    }
    
    return true;
}

void FormatController::applySingleParagraphProperty(const std::function<void(ParagraphStyle&)>& setPropertyFunc)
{
    ParagraphStyle style;
    setPropertyFunc(style);
    applyParagraphStyle(style);
}

} // namespace QtWordEditor