#include "graphics/items/TextBlockItem.h"
#include "graphics/items/TextFragment.h"
#include "graphics/formula/MathFormulaItem.h"
#include "core/document/ParagraphBlock.h"
#include "core/document/ParagraphStyle.h"
#include "core/document/CharacterStyle.h"
#include "core/document/InlineSpan.h"
#include "core/document/TextSpan.h"
#include "core/document/MathSpan.h"
#include "core/utils/Constants.h"
#include <QDebug>
#include <QFont>
#include <QFontMetrics>
#include <QTextDocument>
#include <QTextOption>
#include <QTextCursor>
#include <QTextCharFormat>

namespace QtWordEditor {

TextBlockItem::TextBlockItem(ParagraphBlock *block, QGraphicsItem *parent)
    : BaseBlockItem(block, parent)
    , m_textWidth(Constants::PAGE_WIDTH - 2 * Constants::PAGE_MARGIN)
{
    setFlag(QGraphicsItem::ItemIsSelectable, false);
    setFlag(QGraphicsItem::ItemIsFocusable, false);
    
    // 设置布局引擎
    m_layoutEngine.setAvailableWidth(m_textWidth);
    m_layoutEngine.setWrapMode(TextBlockLayoutEngine::WrapMode::WrapAnywhere);
    
    // 从 ParagraphBlock 读取初始内容
    if (block) {
        m_layoutEngine.setParagraphStyle(block->paragraphStyle());
        createContentItemsFromBlock();
        performLayout();
    }
}

TextBlockItem::~TextBlockItem()
{
    clearContentItems();
}

void TextBlockItem::updateBlock()
{
    ParagraphBlock *para = qobject_cast<ParagraphBlock*>(m_block);
    if (!para)
        return;
    
    // 设置段落样式
    m_layoutEngine.setParagraphStyle(para->paragraphStyle());
    
    // 重新创建内容项并布局
    createContentItemsFromBlock();
    performLayout();
}

void TextBlockItem::setTextWidth(qreal width)
{
    if (m_textWidth != width) {
        m_textWidth = width;
        m_layoutEngine.setAvailableWidth(width);
        performLayout();
    }
}

qreal TextBlockItem::textWidth() const
{
    return m_textWidth;
}

QRectF TextBlockItem::boundingRect() const
{
    return m_boundingRect;
}

void TextBlockItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(painter);
    Q_UNUSED(option);
    Q_UNUSED(widget);
}

void TextBlockItem::updateGeometry()
{
    performLayout();
}

TextFragment* TextBlockItem::getTextFragmentForSpan(InlineSpan* span) const
{
    return m_textFragmentMap.value(span, nullptr);
}

MathFormulaItem* TextBlockItem::getMathItemForSpan(MathSpan* span) const
{
    return m_mathItemMap.value(span, nullptr);
}

void TextBlockItem::createContentItemsFromBlock()
{
    // 清除旧的内容项
    clearContentItems();
    
    ParagraphBlock *para = qobject_cast<ParagraphBlock*>(m_block);
    if (!para)
        return;
    
    // 使用新的 m_inlineSpans 来创建内容项
    for (int i = 0; i < para->inlineSpanCount(); ++i) {
        InlineSpan *span = para->inlineSpan(i);
        
        if (span->type() == InlineSpan::Text) {
            TextSpan *textSpan = qobject_cast<TextSpan*>(span);
            if (textSpan) {
                CharacterStyle style = textSpan->style();
                
                // 创建 TextFragment
                TextFragment *textFragment = new TextFragment(textSpan->text(), style, this);
                m_contentItems.append(textFragment);
                m_textFragmentMap.insert(textSpan, textFragment);
            }
        } else if (span->type() == InlineSpan::Math) {
            MathSpan *mathSpan = qobject_cast<MathSpan*>(span);
            if (mathSpan) {
                // TODO: 后续实现 MathFormulaItem 的创建
                // MathFormulaItem *mathItem = MathFormulaItemFactory::create(mathSpan, this);
                // m_contentItems.append(mathItem);
                // m_mathItemMap.insert(mathSpan, mathItem);
            }
        }
    }
}

void TextBlockItem::performLayout()
{
    // 执行布局
    m_layoutEngine.layout(m_contentItems);
    
    // 设置所有项的位置
    for (QGraphicsItem *item : m_contentItems) {
        QPointF pos = m_layoutEngine.positionForItem(item);
        item->setPos(pos);
    }
    
    // 更新边界矩形
    qreal totalWidth = m_layoutEngine.totalWidth();
    qreal totalHeight = m_layoutEngine.totalHeight();
    setRect(0, 0, totalWidth, totalHeight);
}

void TextBlockItem::clearContentItems()
{
    qDeleteAll(m_contentItems);
    m_contentItems.clear();
    m_textFragmentMap.clear();
    m_mathItemMap.clear();
}

} // namespace QtWordEditor
