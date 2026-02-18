#include "core/styles/StyleManager.h"
#include "core/document/Document.h"
#include "core/commands/SetCharacterStyleCommand.h"
#include "core/commands/SetParagraphStyleCommand.h"
#include <QDebug>
#include <QSet>

namespace QtWordEditor {

StyleManager::StyleManager(QObject *parent)
    : QObject(parent)
{
    // 初始化默认字符样式
    initializeDefaultCharacterStyles();
    
    // 初始化默认段落样式
    initializeDefaultParagraphStyles();
}

void StyleManager::initializeDefaultCharacterStyles()
{
    // Default 样式
    CharacterStyle defaultStyle;
    defaultStyle.setFontSize(12);
    addCharacterStyle("Default", defaultStyle);
    
    // Heading 1 样式
    CharacterStyle heading1;
    heading1.setFontSize(24);
    heading1.setBold(true);
    addCharacterStyle("Heading 1", heading1);
    
    // Heading 2 样式（继承自 Heading 1）
    CharacterStyle heading2;
    heading2.setFontSize(20);
    addCharacterStyle("Heading 2", heading2, "Heading 1");
    
    // Heading 3 样式（继承自 Heading 1）
    CharacterStyle heading3;
    heading3.setFontSize(18);
    addCharacterStyle("Heading 3", heading3, "Heading 1");
    
    // Emphasis 样式
    CharacterStyle emphasis;
    emphasis.setItalic(true);
    addCharacterStyle("Emphasis", emphasis);
    
    // Strong 样式
    CharacterStyle strong;
    strong.setBold(true);
    addCharacterStyle("Strong", strong);
}

void StyleManager::initializeDefaultParagraphStyles()
{
    // Default 样式
    ParagraphStyle defaultStyle;
    addParagraphStyle("Default", defaultStyle);
    
    // Heading 1 段落样式
    ParagraphStyle heading1;
    heading1.setSpaceBefore(12);
    heading1.setSpaceAfter(12);
    addParagraphStyle("Heading 1", heading1);
    
    // Heading 2 段落样式（继承自 Heading 1）
    ParagraphStyle heading2;
    heading2.setSpaceBefore(10);
    heading2.setSpaceAfter(10);
    addParagraphStyle("Heading 2", heading2, "Heading 1");
    
    // Quote 段落样式
    ParagraphStyle quote;
    quote.setLeftIndent(30);
    quote.setRightIndent(30);
    addParagraphStyle("Quote", quote);
}

StyleManager::~StyleManager()
{
}

void StyleManager::setDocument(Document *document)
{
    if (m_document == document)
        return;
    m_document = document;
    // Load default styles?
}

Document *StyleManager::document() const
{
    return m_document;
}

void StyleManager::addCharacterStyle(const QString &name, const CharacterStyle &style, const QString &parentStyleName)
{
    CharacterStyleInfo info;
    info.style = style;
    info.parentStyleName = parentStyleName;
    m_characterStyles.insert(name, info);
    emit stylesChanged();
    emit characterStyleChanged(name);
}

CharacterStyle StyleManager::characterStyle(const QString &name) const
{
    auto it = m_characterStyles.find(name);
    if (it != m_characterStyles.end()) {
        return it->style;
    }
    return CharacterStyle();
}

CharacterStyle StyleManager::getResolvedCharacterStyle(const QString &name) const
{
    CharacterStyle result;
    QSet<QString> visited;  // 防止循环继承
    QString currentStyleName = name;
    
    // 从当前样式开始，沿着继承链向上收集所有父样式
    QList<CharacterStyle> styleChain;
    
    while (!currentStyleName.isEmpty() && !visited.contains(currentStyleName)) {
        visited.insert(currentStyleName);
        
        auto it = m_characterStyles.find(currentStyleName);
        if (it == m_characterStyles.end()) {
            break;
        }
        
        // 将当前样式添加到链中（最后合并时顺序：父样式在前，子样式在后）
        styleChain.prepend(it->style);
        
        // 继续处理父样式
        currentStyleName = it->parentStyleName;
    }
    
    // 合并所有样式（子样式覆盖父样式）
    for (const auto &style : styleChain) {
        result = result.mergeWith(style);
    }
    
    return result;
}

bool StyleManager::hasCharacterStyle(const QString &name) const
{
    return m_characterStyles.contains(name);
}

QStringList StyleManager::characterStyleNames() const
{
    return m_characterStyles.keys();
}

void StyleManager::setCharacterStyleParent(const QString &styleName, const QString &parentStyleName)
{
    auto it = m_characterStyles.find(styleName);
    if (it != m_characterStyles.end()) {
        it->parentStyleName = parentStyleName;
        emit stylesChanged();
        emit characterStyleChanged(styleName);
    }
}

QString StyleManager::characterStyleParent(const QString &styleName) const
{
    auto it = m_characterStyles.find(styleName);
    if (it != m_characterStyles.end()) {
        return it->parentStyleName;
    }
    return QString();
}

void StyleManager::addParagraphStyle(const QString &name, const ParagraphStyle &style, const QString &parentStyleName)
{
    ParagraphStyleInfo info;
    info.style = style;
    info.parentStyleName = parentStyleName;
    m_paragraphStyles.insert(name, info);
    emit stylesChanged();
    emit paragraphStyleChanged(name);
}

ParagraphStyle StyleManager::paragraphStyle(const QString &name) const
{
    auto it = m_paragraphStyles.find(name);
    if (it != m_paragraphStyles.end()) {
        return it->style;
    }
    return ParagraphStyle();
}

ParagraphStyle StyleManager::getResolvedParagraphStyle(const QString &name) const
{
    ParagraphStyle result;
    QSet<QString> visited;  // 防止循环继承
    QString currentStyleName = name;
    
    // 从当前样式开始，沿着继承链向上收集所有父样式
    QList<ParagraphStyle> styleChain;
    
    while (!currentStyleName.isEmpty() && !visited.contains(currentStyleName)) {
        visited.insert(currentStyleName);
        
        auto it = m_paragraphStyles.find(currentStyleName);
        if (it == m_paragraphStyles.end()) {
            break;
        }
        
        // 将当前样式添加到链中（最后合并时顺序：父样式在前，子样式在后）
        styleChain.prepend(it->style);
        
        // 继续处理父样式
        currentStyleName = it->parentStyleName;
    }
    
    // 合并所有样式（子样式覆盖父样式）
    for (const auto &style : styleChain) {
        result = result.mergeWith(style);
    }
    
    return result;
}

bool StyleManager::hasParagraphStyle(const QString &name) const
{
    return m_paragraphStyles.contains(name);
}

QStringList StyleManager::paragraphStyleNames() const
{
    return m_paragraphStyles.keys();
}

void StyleManager::setParagraphStyleParent(const QString &styleName, const QString &parentStyleName)
{
    auto it = m_paragraphStyles.find(styleName);
    if (it != m_paragraphStyles.end()) {
        it->parentStyleName = parentStyleName;
        emit stylesChanged();
        emit paragraphStyleChanged(styleName);
    }
}

QString StyleManager::paragraphStyleParent(const QString &styleName) const
{
    auto it = m_paragraphStyles.find(styleName);
    if (it != m_paragraphStyles.end()) {
        return it->parentStyleName;
    }
    return QString();
}

void StyleManager::applyCharacterStyle(const QString &styleName, int blockIndex, int start, int end)
{
    // 检查样式是否存在
    if (!hasCharacterStyle(styleName))
        return;
    
    // 检查文档是否有效
    if (!m_document)
        return;
    
    // 获取解析继承后的样式对象
    CharacterStyle style = getResolvedCharacterStyle(styleName);
    
    // 创建命令并推入撤销栈
    SetCharacterStyleCommand *cmd = new SetCharacterStyleCommand(
        m_document, blockIndex, start, end, style);
    m_document->undoStack()->push(cmd);
}

void StyleManager::applyParagraphStyle(const QString &styleName, const QList<int> &blockIndices)
{
    // 检查样式是否存在
    if (!hasParagraphStyle(styleName))
        return;
    
    // 检查文档是否有效
    if (!m_document)
        return;
    
    // 检查块索引列表是否为空
    if (blockIndices.isEmpty())
        return;
    
    // 获取解析继承后的样式对象
    ParagraphStyle style = getResolvedParagraphStyle(styleName);
    
    // 创建命令并推入撤销栈
    SetParagraphStyleCommand *cmd = new SetParagraphStyleCommand(
        m_document, blockIndices, style);
    m_document->undoStack()->push(cmd);
}

} // namespace QtWordEditor