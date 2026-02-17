#ifndef FORMATCONTROLLER_H
#define FORMATCONTROLLER_H

#include <QObject>
#include "core/document/CharacterStyle.h"
#include "core/document/ParagraphStyle.h"
#include "core/Global.h"

namespace QtWordEditor {

class Document;
class Selection;

/**
 * @brief The FormatController class provides high‑level formatting APIs.
 */
class FormatController : public QObject
{
    Q_OBJECT
public:
    explicit FormatController(Document *document, Selection *selection = nullptr,
                              QObject *parent = nullptr);
    ~FormatController() override;

    // Character formatting
    void applyCharacterStyle(const CharacterStyle &style);
    void setFont(const QFont &font);
    void setFontSize(int size);
    void setBold(bool bold);
    void setItalic(bool italic);
    void setUnderline(bool underline);
    void setTextColor(const QColor &color);
    void setBackgroundColor(const QColor &color);

    // Paragraph formatting
    void applyParagraphStyle(const ParagraphStyle &style);
    void setAlignment(ParagraphAlignment align);
    void setLeftIndent(qreal indent);
    void setRightIndent(qreal indent);
    void setFirstLineIndent(qreal indent);
    void setLineHeight(int percent);
    void setSpaceBefore(qreal space);
    void setSpaceAfter(qreal space);

private:
    Document *m_document;
    Selection *m_selection;
};

} // namespace QtWordEditor

#endif // FORMATCONTROLLER_H