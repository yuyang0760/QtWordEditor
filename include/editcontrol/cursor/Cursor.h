#ifndef CURSOR_H
#define CURSOR_H

#include <QObject>
#include <QPointF>
#include "core/Global.h"
#include "core/document/CharacterStyle.h"

namespace QtWordEditor {

struct CursorPosition
{
    int blockIndex = -1;
    int offset = 0; // character offset within the block

    bool operator==(const CursorPosition &other) const {
        return blockIndex == other.blockIndex && offset == other.offset;
    }
    bool operator!=(const CursorPosition &other) const {
        return !(*this == other);
    }
};

class Document;

/**
 * @brief The Cursor class manages the insertion point within a document.
 */
class Cursor : public QObject
{
    Q_OBJECT
public:
    explicit Cursor(Document *document, QObject *parent = nullptr);
    ~Cursor() override;

    Document *document() const;

    CursorPosition position() const;
    void setPosition(int blockIndex, int offset);
    void setPosition(const CursorPosition &pos);

    // Movement
    void moveLeft();
    void moveRight();
    void moveUp();
    void moveDown();
    void moveToStartOfLine();
    void moveToEndOfLine();
    void moveToStartOfDocument();
    void moveToEndOfDocument();

    // Editing operations (create commands)
    void insertText(const QString &text, const CharacterStyle &style);
    void deletePreviousChar();
    void deleteNextChar();

signals:
    void positionChanged(const CursorPosition &pos);

private:
    Document *m_document;
    CursorPosition m_position;
};

} // namespace QtWordEditor

Q_DECLARE_METATYPE(QtWordEditor::CursorPosition)

#endif // CURSOR_H