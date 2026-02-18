#include "core/styles/StyleManager.h"
#include "core/document/Document.h"
#include "core/commands/SetCharacterStyleCommand.h"
#include "core/commands/SetParagraphStyleCommand.h"
#include <QDebug>

namespace QtWordEditor {

StyleManager::StyleManager(QObject *parent)
    : QObject(parent)
{
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

void StyleManager::addCharacterStyle(const QString &name, const CharacterStyle &style)
{
    m_characterStyles.insert(name, style);
    emit stylesChanged();
}

CharacterStyle StyleManager::characterStyle(const QString &name) const
{
    return m_characterStyles.value(name, CharacterStyle());
}

bool StyleManager::hasCharacterStyle(const QString &name) const
{
    return m_characterStyles.contains(name);
}

QStringList StyleManager::characterStyleNames() const
{
    return m_characterStyles.keys();
}

void StyleManager::addParagraphStyle(const QString &name, const ParagraphStyle &style)
{
    m_paragraphStyles.insert(name, style);
    emit stylesChanged();
}

ParagraphStyle StyleManager::paragraphStyle(const QString &name) const
{
    return m_paragraphStyles.value(name, ParagraphStyle());
}

bool StyleManager::hasParagraphStyle(const QString &name) const
{
    return m_paragraphStyles.contains(name);
}

QStringList StyleManager::paragraphStyleNames() const
{
    return m_paragraphStyles.keys();
}

void StyleManager::applyCharacterStyle(const QString &styleName, int blockIndex, int start, int end)
{
    // 检查样式是否存在
    if (!hasCharacterStyle(styleName))
        return;
    
    // 检查文档是否有效
    if (!m_document)
        return;
    
    // 获取样式对象
    CharacterStyle style = characterStyle(styleName);
    
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
    
    // 获取样式对象
    ParagraphStyle style = paragraphStyle(styleName);
    
    // 创建命令并推入撤销栈
    SetParagraphStyleCommand *cmd = new SetParagraphStyleCommand(
        m_document, blockIndices, style);
    m_document->undoStack()->push(cmd);
}

} // namespace QtWordEditor