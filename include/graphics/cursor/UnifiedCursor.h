#ifndef UNIFIEDCURSOR_H
#define UNIFIEDCURSOR_H

#include <QObject>
#include <QPointF>
#include <QGraphicsRectItem>
#include "core/Global.h"

namespace QtWordEditor {

class DocumentView;
class ParagraphBlock;
class InlineSpan;
class MathSpan;
class TextBlockItem;
class CharacterStyle;

/**
 * @brief 光标状态结构体
 */
struct CursorState {
    ParagraphBlock* paragraph;
    int spanIndex;
    int charOffset;
    bool isMath;
    MathSpan* mathSpan;
    int mathChildIndex;
    
    CursorState()
        : paragraph(nullptr)
        , spanIndex(0)
        , charOffset(0)
        , isMath(false)
        , mathSpan(nullptr)
        , mathChildIndex(0)
    {}
};

class UnifiedCursor : public QObject
{
    Q_OBJECT
public:
    explicit UnifiedCursor(DocumentView* view, QObject *parent = nullptr);
    ~UnifiedCursor() override;

    void moveLeft();
    void moveRight();
    void moveUp();
    void moveDown();
    void moveToStartOfLine();
    void moveToEndOfLine();
    void moveToStartOfDocument();
    void moveToEndOfDocument();
    
    void setPosition(ParagraphBlock* paragraph, int spanIndex, int charOffset);
    void setPosition(MathSpan* mathSpan, int childIndex);
    void setPositionFromScenePoint(const QPointF& scenePos);
    
    void insertText(const QString& text);
    void insertText(const QString& text, const CharacterStyle& style);
    void deleteChar();
    void deletePreviousChar();
    void backspace();
    
    CursorState state() const;
    bool isInMath() const;
    
    void updateCursor();
    void updateCursor(const QPointF& position, qreal height);
    QGraphicsRectItem* cursorItem() const;

signals:
    void positionChanged();
    void contentChanged();

private:
    TextBlockItem* getCurrentTextBlockItem() const;
    int getCurrentBlockIndex() const;
    void initializeCursor();

private:
    DocumentView* m_view;
    CursorState m_state;
    QGraphicsRectItem* m_cursorItem;
};

} // namespace QtWordEditor

#endif // UNIFIEDCURSOR_H
