#ifndef TEXTBLOCKITEM_H
#define TEXTBLOCKITEM_H

#include "BaseBlockItem.h"
#include <QGraphicsTextItem>
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

private:
    QGraphicsTextItem *m_textItem;
};

} // namespace QtWordEditor

#endif // TEXTBLOCKITEM_H