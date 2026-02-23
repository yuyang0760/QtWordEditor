#include "graphics/scene/DocumentScene.h"
#include "core/document/Document.h"
#include "core/document/Section.h"
#include "core/document/Block.h"
#include "core/document/ParagraphBlock.h"
#include "core/document/Page.h"
#include "core/document/ParagraphStyle.h"
#include "core/utils/Constants.h"
#include "graphics/items/BaseBlockItem.h"
#include "graphics/items/TextBlockItem.h"
#include "graphics/items/CursorItem.h"
#include "graphics/items/SelectionItem.h"
#include "graphics/items/PageItem.h"
#include "editcontrol/cursor/Cursor.h"
#include "editcontrol/selection/Selection.h"
#include <QDebug>
#include <QGraphicsItem>
#include <QGraphicsTextItem>
#include <QTextDocument>
#include <QTextCursor>
#include <QTextBlock>
#include <QTextOption>
#include <QAbstractTextDocumentLayout>
#include <QFontMetrics>
#include <QTextLayout>
#include <QTextLine>

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
    // m_pageTextItems.clear();
  //  QDebug() << "DocumentScene::rebuildFromDocument() - 开始重建场景";
  //  QDebug() << "  文档指针:" << m_document;

    if (m_document) {
      //  QDebug() << "  文档节数量:" << m_document->sectionCount();

        for (int sectionIdx = 0; sectionIdx < m_document->sectionCount(); ++sectionIdx) {
            Section *section = m_document->section(sectionIdx);
          //  QDebug() << "  节" << sectionIdx << "指针:" << section;
            
            if (!section)
                continue;

          //  QDebug() << "  节页面数量:" << section->pageCount();
            for (int pageIdx = 0; pageIdx < section->pageCount(); ++pageIdx) {
                Page *page = section->page(pageIdx);
              //  QDebug() << "  页面" << pageIdx << "指针:" << page;
                if (!page)
                    continue;

                addPage(page);
                
                // 记录当前页的所有文本块项，用于后续计算位置
                QVector<TextBlockItem*> pageBlockItems;
                
                for (int blockIdx = 0; blockIdx < page->blockCount(); ++blockIdx) {
                    Block *block = page->block(blockIdx);
                  //  QDebug() << "    块" << blockIdx << "指针:" << block;
                    if (!block)
                        continue;

                    ParagraphBlock *paraBlock = qobject_cast<ParagraphBlock*>(block);
                    if (paraBlock) {
                      //  QDebug() << ">>>>>>>>>> 添加文本块项，文本内容:" << paraBlock->text();
                        
                        // 创建 TextBlockItem
                        TextBlockItem *textBlockItem = new TextBlockItem(paraBlock);
                        
                        // 连接 ParagraphBlock 的 textChanged 信号，当内容变化时更新 TextBlockItem
                        connect(paraBlock, &ParagraphBlock::textChanged, this, [this, block]() {
                            updateSingleTextItem(block);
                        });
                        
                        // 添加到场景
                        addItem(textBlockItem);
                        
                        // 添加到 m_blockItems 和 pageBlockItems
                        m_blockItems.insert(block, textBlockItem);
                        pageBlockItems.append(textBlockItem);
                        
                      //  QDebug() << ">>>>>>>>>>      文本项边界矩形:" << textBlockItem->boundingRect();
                    }
                }
                
                // 根据每个块的实际高度计算位置，考虑段前和段后间距
                qreal currentY = Constants::PAGE_MARGIN;
                for (int i = 0; i < pageBlockItems.size(); ++i) {
                    TextBlockItem *textBlockItem = pageBlockItems[i];
                    ParagraphBlock *paraBlock = qobject_cast<ParagraphBlock*>(textBlockItem->block());
                    
                    qreal textX = Constants::PAGE_MARGIN;
                    qreal spaceBefore = 0.0;
                    
                    // 只有第一个块之后的块才添加段前间距
                    if (i > 0 && paraBlock) {
                        spaceBefore = paraBlock->paragraphStyle().spaceBefore();
                    }
                    
                    // 应用段前间距
                    currentY += spaceBefore;
                    
                    // 设置块的位置
                    textBlockItem->setPos(textX, currentY);
                    
                    // 下一个块从当前块的底部开始，加上段后间距
                    qreal blockHeight = textBlockItem->boundingRect().height();
                    qreal spaceAfter = paraBlock ? paraBlock->paragraphStyle().spaceAfter() : 0.0;
                    currentY += blockHeight + spaceAfter;
                }
            }
        }
    }
    
    // 重新添加光标和选择项
    if (tempCursor) {
        addItem(tempCursor);
        m_cursorItem = tempCursor;
      //  QDebug() << "  恢复光标项";
    }
    if (tempSelection) {
        addItem(tempSelection);
        m_selectionItem = tempSelection;
      //  QDebug() << "  恢复选择项";
    }
    
  //  QDebug() << "DocumentScene::rebuildFromDocument() - 场景重建完成！";
}

void DocumentScene::updateAllTextItems()
{
  //  QDebug() << "DocumentScene::updateAllTextItems() - 更新所有文本项";
    for (BaseBlockItem *item : m_blockItems) {
        if (item) {
            item->updateBlock();
        }
    }
    // 更新所有块的位置
    updateBlockPositions();
}

void DocumentScene::updateBlockPositions()
{
    if (!m_document) {
        return;
    }

    // 遍历文档中的所有节
    for (int sectionIdx = 0; sectionIdx < m_document->sectionCount(); ++sectionIdx) {
        Section *section = m_document->section(sectionIdx);
        if (!section) {
            continue;
        }

        // 遍历节中的所有页面
        for (int pageIdx = 0; pageIdx < section->pageCount(); ++pageIdx) {
            Page *page = section->page(pageIdx);
            if (!page) {
                continue;
            }

            // 收集当前页面的所有文本块项
            QVector<TextBlockItem*> pageBlockItems;
            for (int blockIdx = 0; blockIdx < page->blockCount(); ++blockIdx) {
                Block *block = page->block(blockIdx);
                if (!block) {
                    continue;
                }
                auto it = m_blockItems.find(block);
                if (it != m_blockItems.end()) {
                    TextBlockItem *textBlockItem = dynamic_cast<TextBlockItem*>(it.value());
                    if (textBlockItem) {
                        pageBlockItems.append(textBlockItem);
                    }
                }
            }

            // 根据每个块的实际高度重新计算位置，考虑段前和段后间距
            qreal currentY = Constants::PAGE_MARGIN;
            for (int i = 0; i < pageBlockItems.size(); ++i) {
                TextBlockItem *textBlockItem = pageBlockItems[i];
                ParagraphBlock *paraBlock = qobject_cast<ParagraphBlock*>(textBlockItem->block());
                
                qreal textX = Constants::PAGE_MARGIN;
                qreal spaceBefore = 0.0;
                
                // 只有第一个块之后的块才添加段前间距
                if (i > 0 && paraBlock) {
                    spaceBefore = paraBlock->paragraphStyle().spaceBefore();
                }
                
                // 应用段前间距
                currentY += spaceBefore;
                
                // 设置块的位置
                textBlockItem->setPos(textX, currentY);
                
                // 下一个块从当前块的底部开始，加上段后间距
                qreal blockHeight = textBlockItem->boundingRect().height();
                qreal spaceAfter = paraBlock ? paraBlock->paragraphStyle().spaceAfter() : 0.0;
                currentY += blockHeight + spaceAfter;
            }
        }
    }
}

void DocumentScene::updateSingleTextItem(Block *block)
{
    qDebug() << "[DEBUG] DocumentScene::updateSingleTextItem - block:" << block;
    if (!block)
        return;
    auto it = m_blockItems.find(block);
    if (it != m_blockItems.end() && it.value()) {
        qDebug() << "[DEBUG] DocumentScene::updateSingleTextItem - calling updateBlock() on item:" << it.value();
        it.value()->updateBlock();
    }
    // 更新所有块的位置
    updateBlockPositions();
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
    
  //  QDebug() << "添加页面" << page->pageNumber() << "，Y坐标:" << yOffset;
}

void DocumentScene::updateCursor(const QPointF &pos, qreal height)
{
  //  QDebug() << "DocumentScene::updateCursor - 更新光标，位置:" << pos << "，高度:" << height;
    if (!m_cursorItem) {
      //  QDebug() << "  创建新的光标项";
        m_cursorItem = new CursorItem();
        addItem(m_cursorItem);
    }
    m_cursorItem->setPosition(pos, height);
}

// ========== 新增方法：设置光标可见性 ==========
void DocumentScene::setCursorVisible(bool visible)
{
    if (m_cursorItem) {
        m_cursorItem->setCursorVisible(visible);
    }
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

CursorPosition DocumentScene::cursorPositionAt(const QPointF &scenePos) const
{
    CursorPosition pos;
    pos.blockIndex = 0;
    pos.offset = 0;
    
    if (!m_document) {
        return pos;
    }
    
    // 先找到所在的页
    Page *page = pageAt(scenePos);
    if (!page) {
        return pos;
    }
    
    // 简单计算：根据Y坐标计算块索引
    qreal pageSpacing = 30.0;
    qreal pageHeight = Constants::PAGE_HEIGHT;
    int pageIndex = page->pageNumber() - 1;
    qreal yOffset = pageIndex * (pageHeight + pageSpacing);
    
    qreal relativeY = scenePos.y() - yOffset - Constants::PAGE_MARGIN;
    int blockIndex = qBound(0, qFloor(relativeY / 30.0), page->blockCount() - 1);
    pos.blockIndex = blockIndex;
    
    // TODO: 实现使用新 TextBlockItem 的光标定位
    // 暂时使用偏移0
    pos.offset = 0;
    
  //  QDebug() << "DocumentScene::cursorPositionAt - 场景位置:" << scenePos
  //           << "→ 块索引:" << pos.blockIndex << "，偏移:" << pos.offset;
    
    return pos;
}

QPointF DocumentScene::calculateCursorVisualPosition(const CursorPosition &pos) const
{
    QPointF result(0, 0);
  //  QDebug() << "DocumentScene::calculateCursorVisualPosition - 计算光标位置，块:" << pos.blockIndex << "，偏移:" << pos.offset;
    
    if (!m_document) {
      //  QDebug() << "  文档指针为空！";
        return result;
    }
    
    // 找到所在的页（简单假设：第0个section）
    Section *section = m_document->section(0);
    if (!section) {
      //  QDebug() << "  节指针为空！";
        return result;
    }
    
    // 找到对应页的index：这里简化，假设每个section只有1页
    int pageIndex = 0;
    if (pageIndex < 0 || pageIndex >= section->pageCount()) {
      //  QDebug() << "  页面索引超出范围！";
        return result;
    }
    
    Page *page = section->page(pageIndex);
    if (!page) {
      //  QDebug() << "  页面指针为空！";
        return result;
    }
    
    // TODO: 实现使用新 TextBlockItem 的光标位置计算
    // 暂时返回简单位置
    
    // 先找到块的 TextBlockItem
    Block *block = page->block(pos.blockIndex);
    if (block) {
        auto it = m_blockItems.find(block);
        if (it != m_blockItems.end()) {
            TextBlockItem *textBlockItem = dynamic_cast<TextBlockItem*>(it.value());
            if (textBlockItem) {
                result = textBlockItem->scenePos();
                result.setY(result.y() + 5); // 简单的偏移
            }
        }
    }
    
  //  QDebug() << "  返回结果坐标:" << result;
    
    return result;
}

QList<QRectF> DocumentScene::calculateSelectionRects(const SelectionRange &range) const
{
    QList<QRectF> rects;
    
    if (!m_document) {
        return rects;
    }
    
    // TODO: 实现使用新 TextBlockItem 的选择矩形计算
    // 暂时返回空列表
    
    return rects;
}

BaseBlockItem* DocumentScene::blockItemForBlock(Block* block) const
{
    if (!block) {
        return nullptr;
    }
    
    auto it = m_blockItems.find(block);
    if (it != m_blockItems.end()) {
        return it.value();
    }
    
    return nullptr;
}

} // namespace QtWordEditor
