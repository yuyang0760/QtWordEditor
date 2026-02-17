#include "FormatController.h"
#include "core/document/Document.h"
#include "editcontrol/selection/Selection.h"
#include "core/commands/SetCharacterStyleCommand.h"
#include <QDebug>

namespace QtWordEditor {

FormatController::FormatController(Document *document, Selection *selection, QObject *parent)
    : QObject(parent)
    , m_document(document)
    , m_selection(selection)
{
}

FormatController::~FormatController()
{
}

void FormatController::applyCharacterStyle(const CharacterStyle &style)
{
    if (!m_document)
        return;
    // Apply to current selection or to the whole document?
    // For now, placeholder
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

void FormatController::applyParagraphStyle(const ParagraphStyle &style)
{
    Q_UNUSED(style);
    // TODO: implement
}

void FormatController::setAlignment(ParagraphAlignment align)
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

} // namespace QtWordEditor