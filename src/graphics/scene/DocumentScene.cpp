#include "graphics/scene/DocumentScene.h"
#include "core/document/Document.h"
#include "core/document/Section.h"
#include "core/document/Block.h"
#include "core/document/ParagraphBlock.h"
#include "core/document/Page.h"
#include "core/utils/Constants.h"
#include "graphics/items/BaseBlockItem.h"
#include "graphics/items/TextBlockItem.h"
#include "graphics/items/CursorItem.h"
#include "graphics/items/SelectionItem.h"
#include "graphics/items/PageItem.h"
#include <QDebug>
#include <QGraphicsItem>
#include <QGraphicsTextItem>
#include <QTextDocument>

namespace QtWordEditor {

DocumentScene::DocumentScene(QObject *parent)
    : QGraphicsScene(parent)
    , m_document(nullptr)
    , m_cursorItem(nullptr)
    , m_selectionItem(nullptr)
{
    setBackgroundBrush(QBrush(QColor(200, 200, 200)));
}

DocumentScene::~DocumentScene()
{
}

void DocumentScene::setDocument(Document *document)
{
    if (m_document == document)
        return;
    m_document = document;
    rebuildFromDocument();
}

Document *DocumentScene::document() const
{
    return m_document;
}

void DocumentScene::rebuildFromDocument()
{
    // 先临时保存光标和选择项，避免被 clear() 删除
    CursorItem *tempCursor = m_cursorItem;
    SelectionItem *tempSelection = m_selectionItem;
    
    // 移除这些项目，但不删除它们
    if (tempCursor) removeItem(tempCursor);
    if (tempSelection) removeItem(tempSelection);
    
    clear();
    m_blockItems.clear();
    m_pageItems.clear();
    qDebug() << "DocumentScene::rebuildFromDocument()";
    qDebug() << "  m_document =" << m_document;

    if (m_document) {
        qDebug() << "  Document::sectionCount =" << m_document->sectionCount();

        for (int sectionIdx = 0; sectionIdx < m_document->sectionCount(); ++sectionIdx) {
            Section *section = m_document->section(sectionIdx);
            qDebug() << "  Section" << sectionIdx << "=" << section;
            
            if (!section)
                continue;

            qDebug() << "  Section::pageCount =" << section->pageCount();
            for (int pageIdx = 0; pageIdx < section->pageCount(); ++pageIdx) {
                Page *page = section->page(pageIdx);
                qDebug() << "  Page" << pageIdx << "=" << page;
                if (!page)
                    continue;

                addPage(page);
                
                for (int blockIdx = 0; blockIdx < page->blockCount(); ++blockIdx) {
                    Block *block = page->block(blockIdx);
                    qDebug() << "    Block" << blockIdx << "=" << block;
                    if (!block)
                        continue;

                    ParagraphBlock *paraBlock = qobject_cast<ParagraphBlock*>(block);
                    if (paraBlock) {
                        qDebug() << ">>>>>>>>>> Adding QGraphicsTextItem for block, text:" << paraBlock->text();
                        
                        QGraphicsTextItem *textItem = new QGraphicsTextItem();
                        if (paraBlock->text().isEmpty()) {
                            textItem->setPlainText("");
                        } else {
                            textItem->setPlainText(paraBlock->text());
                        }
                        QFont font;
                        font.setPointSize(12);
                        textItem->setFont(font);
                        
                        // 禁用 QGraphicsTextItem 的默认文档边距
                        textItem->document()->setDocumentMargin(0);
                        
                        // 让我们调整一下位置，确保文本和光标位置一致
                        // QGraphicsTextItem 的原点默认在左上角
                        qreal textX = Constants::PAGE_MARGIN;
                        qreal textY = Constants::PAGE_MARGIN + blockIdx * 30;
                        textItem->setPos(textX, textY);
                        addItem(textItem);
                        
                        qDebug() << ">>>>>>>>>>      TextItem pos:" << textX << "," << textY;
                        qDebug() << ">>>>>>>>>>      TextItem boundingRect:" << textItem->boundingRect();
                    }
                }
            }
        }
    }
    
    // 重新添加光标和选择项
    if (tempCursor) {
        addItem(tempCursor);
        m_cursorItem = tempCursor;
        qDebug() << "  Restored CursorItem";
    }
    if (tempSelection) {
        addItem(tempSelection);
        m_selectionItem = tempSelection;
        qDebug() << "  Restored SelectionItem";
    }
    
    qDebug() << "DocumentScene::rebuildFromDocument() done!";
}

void DocumentScene::updateAllTextItems()
{
    qDebug() << "DocumentScene::updateAllTextItems()";
    for (BaseBlockItem *item : m_blockItems) {
        if (item) {
            item->updateBlock();
        }
    }
}

void DocumentScene::clearPages()
{
    qDeleteAll(m_pageItems);
    m_pageItems.clear();
}

void DocumentScene::addPage(Page *page)
{
    if (!page)
        return;
    
    PageItem *pageItem = new PageItem(page);
    
    qreal pageSpacing = 30.0;
    qreal yOffset = m_pageItems.size() * (page->pageRect().height() + pageSpacing);
    
    pageItem->setPos(0, yOffset);
    addItem(pageItem);
    m_pageItems.append(pageItem);
    
    qreal totalWidth = page->pageRect().width();
    qreal totalHeight = yOffset + page->pageRect().height() + 50.0;
    setSceneRect(-50, -50, totalWidth + 100, totalHeight + 100);
    
    qDebug() << "Added page" << page->pageNumber() << "at y =" << yOffset;
}

void DocumentScene::updateCursor(const QPointF &pos, qreal height)
{
    qDebug() << "DocumentScene::updateCursor called, pos:" << pos << ", height:" << height;
    if (!m_cursorItem) {
        qDebug() << "  Creating new CursorItem";
        m_cursorItem = new CursorItem();
        addItem(m_cursorItem);
    }
    m_cursorItem->setPosition(pos, height);
    m_cursorItem->setVisible(true);
}

void DocumentScene::updateSelection(const QList<QRectF> &rects)
{
    if (!m_selectionItem) {
        m_selectionItem = new SelectionItem();
        addItem(m_selectionItem);
    }
    m_selectionItem->setRects(rects);
}

void DocumentScene::clearSelection()
{
    if (m_selectionItem) {
        m_selectionItem->clear();
    }
}

void DocumentScene::onBlockAdded(int globalIndex)
{
    Q_UNUSED(globalIndex);
}

void DocumentScene::onBlockRemoved(int globalIndex)
{
    Q_UNUSED(globalIndex);
}

void DocumentScene::onLayoutChanged()
{
}

Page *DocumentScene::pageAt(const QPointF &scenePos) const
{
    for (PageItem *pageItem : m_pageItems) {
        if (pageItem && pageItem->page()) {
            QRectF pageRect = pageItem->page()->pageRect();
            QPointF itemPos = pageItem->pos();
            QRectF totalRect(itemPos.x(), itemPos.y(), pageRect.width(), pageRect.height());
            
            if (totalRect.contains(scenePos)) {
                return pageItem->page();
            }
        }
    }
    return nullptr;
}

} // namespace QtWordEditor