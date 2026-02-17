#ifndef STYLEMANAGER_H
#define STYLEMANAGER_H

#include <QObject>
#include <QHash>
#include <QString>
#include "core/document/CharacterStyle.h"
#include "core/document/ParagraphStyle.h"
#include "core/Global.h"

namespace QtWordEditor {

class Document;

/**
 * @brief The StyleManager class manages named styles (character and paragraph).
 */
class StyleManager : public QObject
{
    Q_OBJECT
public:
    explicit StyleManager(QObject *parent = nullptr);
    ~StyleManager() override;

    void setDocument(Document *document);
    Document *document() const;

    // Character styles
    void addCharacterStyle(const QString &name, const CharacterStyle &style);
    CharacterStyle characterStyle(const QString &name) const;
    bool hasCharacterStyle(const QString &name) const;
    QStringList characterStyleNames() const;

    // Paragraph styles
    void addParagraphStyle(const QString &name, const ParagraphStyle &style);
    ParagraphStyle paragraphStyle(const QString &name) const;
    bool hasParagraphStyle(const QString &name) const;
    QStringList paragraphStyleNames() const;

    // Apply styles to selection
    void applyCharacterStyle(const QString &styleName, int blockIndex, int start, int end);
    void applyParagraphStyle(const QString &styleName, const QList<int> &blockIndices);

signals:
    void stylesChanged();

private:
    Document *m_document = nullptr;
    QHash<QString, CharacterStyle> m_characterStyles;
    QHash<QString, ParagraphStyle> m_paragraphStyles;
};

} // namespace QtWordEditor

#endif // STYLEMANAGER_H