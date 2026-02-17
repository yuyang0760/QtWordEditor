#include "core/styles/StyleManager.h"
#include "core/document/Document.h"
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
    Q_UNUSED(blockIndex);
    Q_UNUSED(start);
    Q_UNUSED(end);
    if (!hasCharacterStyle(styleName))
        return;
    // TODO: create a command and push onto undo stack
}

void StyleManager::applyParagraphStyle(const QString &styleName, const QList<int> &blockIndices)
{
    Q_UNUSED(blockIndices);
    if (!hasParagraphStyle(styleName))
        return;
    // TODO: implement
}

} // namespace QtWordEditor