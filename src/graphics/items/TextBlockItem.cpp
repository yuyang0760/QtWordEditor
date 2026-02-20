#include "graphics/items/TextBlockItem.h"
#include "core/document/ParagraphBlock.h"
#include "core/document/ParagraphStyle.h"
#include "core/document/CharacterStyle.h"
#include "core/utils/Constants.h"
#include "graphics/items/TextFragment.h"
#include "graphics/items/TextBlockLayoutEngine.h"
#include <QDebug>
#include <QFont>
#include <QFontMetrics>
#include <QPainter>

namespace QtWordEditor {

TextBlockItem::TextBlockItem(ParagraphBlock *block, QGraphicsItem *parent)
    : BaseBlockItem(block, parent)
    , m_layoutEngine(new TextBlockLayoutEngine())
    , m_textWidth(Constants::PAGE_WIDTH - 2 * Constants::PAGE_MARGIN)
    , m_leftIndent(0)
    , m_rightIndent(0)
{
    setFlag(QGraphicsItem::ItemIsSelectable, false);
    setFlag(QGraphicsItem::ItemIsFocusable, false);
    
    // 从 ParagraphBlock 读取初始文本
    if (block) {
        buildContentItems();
        performLayout();
    }
}

TextBlockItem::~TextBlockItem()
{
    clearContentItems();
    delete m_layoutEngine;
}

QGraphicsTextItem *TextBlockItem::textItem() const
{
    // 为了兼容性保留此接口，但返回 nullptr，因为不再使用 QGraphicsTextItem
    return nullptr;
}

void TextBlockItem::setTextWidth(qreal width)
{
    if (m_textWidth != width) {
        m_textWidth = width;
        performLayout();
    }
}

qreal TextBlockItem::textWidth() const
{
    return m_textWidth;
}

void TextBlockItem::setFont(const QFont &font)
{
    Q_UNUSED(font);
    // 此方法为了兼容性保留，但不再使用
    // 字体现在由 CharacterStyle 管理
}

QFont TextBlockItem::font() const
{
    // 返回默认字体，为了兼容性
    QFont font;
    font.setPointSize(Constants::DefaultFontSize);
    return font;
}

void TextBlockItem::setPlainText(const QString &text)
{
    Q_UNUSED(text);
    // 此方法为了兼容性保留
    // 文本内容现在通过 ParagraphBlock 管理
}

QString TextBlockItem::toPlainText() const
{
    QString result;
    for (QGraphicsItem *item : m_contentItems) {
        TextFragment *fragment = qgraphicsitem_cast<TextFragment*>(item);
        if (fragment) {
            result += fragment->text();
        }
    }
    return result;
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
    // 子项会自动绘制
}

void TextBlockItem::updateGeometry()
{
    performLayout();
}

void TextBlockItem::updateBlock()
{
    ParagraphBlock *para = qobject_cast<ParagraphBlock*>(m_block);
    if (!para)
        return;
    
    buildContentItems();
    applyParagraphIndent();
    performLayout();
}

void TextBlockItem::buildContentItems()
{
    // 清除旧的内容项
    clearContentItems();
    
    ParagraphBlock *para = qobject_cast<ParagraphBlock*>(m_block);
    if (!para)
        return;
    
    qDebug() << "TextBlockItem::buildContentItems - 开始，块ID:" << para->blockId();
    qDebug() << "  spanCount:" << para->spanCount();
    
    // 为每个 Span 创建一个 TextFragment
    for (int i = 0; i < para->spanCount(); ++i) {
        Span span = para->span(i);
        CharacterStyle style = span.style();
        
        qDebug() << "  处理 span " << i << ": text=[" << span.text() << "], 加粗:" << style.bold();
        
        // 创建 TextFragment 并添加到内容项列表
        TextFragment *fragment = new TextFragment(span.text(), style, this);
        m_contentItems << fragment;
    }
    
    qDebug() << "TextBlockItem::buildContentItems - 完成，创建了" << m_contentItems.size() << "个片段";
}

void TextBlockItem::clearContentItems()
{
    // 删除所有内容项
    qDeleteAll(m_contentItems);
    m_contentItems.clear();
}

void TextBlockItem::performLayout()
{
    if (m_contentItems.isEmpty()) {
        m_boundingRect = QRectF(0, 0, m_textWidth, 0);
        return;
    }
    
    ParagraphBlock *para = qobject_cast<ParagraphBlock*>(m_block);
    if (para) {
        m_layoutEngine->setParagraphStyle(para->paragraphStyle());
    }
    
    // 计算可用宽度（减去缩进）
    qreal availableWidth = m_textWidth - m_leftIndent - m_rightIndent;
    if (availableWidth < 10.0) {
        availableWidth = 10.0;
    }
    
    m_layoutEngine->setAvailableWidth(availableWidth);
    m_layoutEngine->setWrapMode(TextBlockLayoutEngine::WrapMode::WrapAnywhere);
    
    // 执行布局
    m_layoutEngine->layout(m_contentItems);
    
    // 定位所有内容项（加上左缩进偏移）
    for (QGraphicsItem *item : m_contentItems) {
        QPointF pos = m_layoutEngine->positionForItem(item);
        item->setPos(pos.x() + m_leftIndent, pos.y());
    }
    
    // 更新边界矩形
    m_boundingRect = QRectF(0, 0, m_textWidth, m_layoutEngine->totalHeight());
    
    qDebug() << "TextBlockItem::performLayout - 完成，boundingRect:" << m_boundingRect;
}

void TextBlockItem::applyParagraphIndent()
{
    ParagraphBlock *para = qobject_cast<ParagraphBlock*>(m_block);
    if (!para) {
        return;
    }
    
    // 获取段落样式中的缩进值
    ParagraphStyle style = para->paragraphStyle();
    m_leftIndent = style.leftIndent();
    m_rightIndent = style.rightIndent();
    
    qDebug() << "TextBlockItem::applyParagraphIndent - 左缩进:" << m_leftIndent << "右缩进:" << m_rightIndent;
}

} // namespace QtWordEditor
