#ifndef GLOBAL_H
#define GLOBAL_H

#include <QtGlobal>
#include <QMetaType>

#ifdef QTWORDEDITOR_CORE_LIBRARY
#define QTWORDEDITOR_CORE_EXPORT Q_DECL_EXPORT
#else
#define QTWORDEDITOR_CORE_EXPORT Q_DECL_IMPORT
#endif

namespace QtWordEditor {

// Forward declarations
class Document;
class Section;
class Block;
class ParagraphBlock;
class ImageBlock;
class TableBlock;
class Span;
class CharacterStyle;
class ParagraphStyle;
class Page;

class EditCommand;
class InsertTextCommand;
class RemoveTextCommand;
class InsertBlockCommand;
class RemoveBlockCommand;
class SetCharacterStyleCommand;
class SetParagraphStyleCommand;

class LayoutEngine;
class PageBuilder;

class StyleManager;

class Cursor;
class Selection;
class EditEventHandler;
class FormatController;

struct CursorPosition;
struct SelectionRange;

// Enumerations
enum ParagraphAlignment {
    AlignLeft,
    AlignCenter,
    AlignRight,
    AlignJustify
};

// Register meta types
inline void registerMetaTypes()
{
    qRegisterMetaType<CursorPosition>("CursorPosition");
    qRegisterMetaType<SelectionRange>("SelectionRange");
    qRegisterMetaType<ParagraphAlignment>("ParagraphAlignment");
}

} // namespace QtWordEditor

Q_DECLARE_METATYPE(QtWordEditor::CursorPosition)
Q_DECLARE_METATYPE(QtWordEditor::SelectionRange)
Q_DECLARE_METATYPE(QtWordEditor::ParagraphAlignment)

#endif // GLOBAL_H