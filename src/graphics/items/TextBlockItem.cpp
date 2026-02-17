#include "graphics/items/TextBlockItem.h"
#include "core/document/ParagraphBlock.h"
#include <QDebug>

namespace QtWordEditor {

TextBlockItem::TextBlockItem(ParagraphBlock *block, QGraphicsItem *parent)
    : BaseBlockItem(block, parent)
    , m_textItem(new QGraphicsTextItem(this))
{
    setFlag(QGraphicsItem::ItemIsSelectable, false);
    setFlag(QGraphicsItem::ItemIsFocusable, false);
}

TextBlockItem::~TextBlockItem()
{
}

void TextBlockItem::updateBlock()
{
    ParagraphBlock *para = qobject_cast<ParagraphBlock*>(m_block);
    if (!para)
        return;

    // TODO: set rich text content and style
    m_textItem->setPlainText(para->text());
    // Adjust position according to block boundingRect
    m_textItem->setPos(m_block->boundingRect().topLeft());
}

} // namespace QtWordEditor