#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <QObject>
#include <QList>
#include <QString>
#include <QDateTime>
#include <QUndoStack>
#include "core/Global.h"

namespace QtWordEditor {

class Section;
class Block;

/**
 * @brief The Document class is the root container of a document.
 *
 * It holds multiple sections, document metadata, and the undo stack.
 */
class Document : public QObject
{
    Q_OBJECT
public:
    explicit Document(QObject *parent = nullptr);
    ~Document() override;

    // Document identification
    int documentId() const;
    void setDocumentId(int id);

    // Metadata
    QString title() const;
    void setTitle(const QString &title);

    QString author() const;
    void setAuthor(const QString &author);

    QDateTime created() const;
    QDateTime modified() const;
    void setModified(const QDateTime &modified);

    // Sections
    int sectionCount() const;
    Section *section(int index) const;
    void addSection(Section *section);
    void insertSection(int index, Section *section);
    void removeSection(int index);

    // Blocks (global index across all sections)
    int blockCount() const;
    Block *block(int globalIndex) const;

    // Undo stack
    QUndoStack *undoStack() const;

    // Plain text export (for testing)
    QString plainText() const;

signals:
    void documentChanged();
    void sectionAdded(int index);
    void sectionRemoved(int index);
    void blockAdded(int globalIndex);
    void blockRemoved(int globalIndex);
    void layoutChanged();

private:
    void updateBlockPositions();

private:
    int m_documentId = -1;
    QString m_title;
    QString m_author;
    QDateTime m_created;
    QDateTime m_modified;
    QList<Section*> m_sections;
    QScopedPointer<QUndoStack> m_undoStack;
};

} // namespace QtWordEditor

#endif // DOCUMENT_H