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

    m_textItem->setPlainText(para->text());
    
    QFont font;
    font.setPointSize(12);
    m_textItem->setFont(font);
    
    QRectF rect = m_block->boundingRect();
    m_textItem->setPos(rect.topLeft());
}

} // namespace QtWordEditor