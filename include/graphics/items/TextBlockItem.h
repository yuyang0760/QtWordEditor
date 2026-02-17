#ifndef TEXTBLOCKITEM_H
#define TEXTBLOCKITEM_H

#include "BaseBlockItem.h"
#include <QGraphicsTextItem>
#include <QFont>
#include "core/Global.h"

namespace QtWordEditor {

class ParagraphBlock;

/**
 * @brief The TextBlockItem class renders a paragraph block as formatted text.
 */
class TextBlockItem : public BaseBlockItem
{
public:
    explicit TextBlockItem(ParagraphBlock *block, QGraphicsItem *parent = nullptr);
    ~TextBlockItem() override;

    void updateBlock() override;
    
    QGraphicsTextItem *textItem() const;
    
    void setTextWidth(qreal width);
    qreal textWidth() const;
    
    void setFont(const QFont &font);
    QFont font() const;
    
    void setPlainText(const QString &text);
    QString toPlainText() const;
    
    QRectF boundingRect() const override;
    
    void updateGeometry();

private:
    void initializeTextItem();
    void updateBoundingRect();
    void applyRichTextFromBlock();
    
    QGraphicsTextItem *m_textItem;
    qreal m_textWidth;
};

} // namespace QtWordEditor

#endif // TEXTBLOCKITEM_H