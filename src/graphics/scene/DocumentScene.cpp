#include "graphics/scene/DocumentScene.h"
#include "core/document/Document.h"
#include "core/document/Section.h"
#include "core/document/Block.h"
#include "core/document/ParagraphBlock.h"
#include "core/document/Page.h"
#include "core/document/ParagraphStyle.h"
#include "core/utils/Constants.h"
#include "core/utils/Logger.h"
#include "graphics/items/BaseBlockItem.h"
#include "graphics/items/TextBlockItem.h"
#include "graphics/items/CursorItem.h"
#include "graphics/items/SelectionItem.h"
#include "graphics/items/PageItem.h"
#include "editcontrol/cursor/Cursor.h"
#include "editcontrol/selection/Selection.h"
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
    m_pageTextItems.clear();
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
                
                // 初始化当前页的文本项列表
                QVector<QGraphicsTextItem*> pageTextItems;
                
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
                        
                        // 设置文本宽度（页面宽度减去左右边距）
                        qreal availableWidth = Constants::PAGE_WIDTH - 2 * Constants::PAGE_MARGIN;
                        textBlockItem->setTextWidth(availableWidth);
                        
                        // 添加到场景
                        addItem(textBlockItem);
                        
                        // 添加到 m_blockItems、m_pageTextItems 和 pageBlockItems
                        m_blockItems.insert(block, textBlockItem);
                        pageTextItems.append(textBlockItem->textItem());
                        pageBlockItems.append(textBlockItem);
                        
                      //  QDebug() << ">>>>>>>>>>      文本项边界矩形:" << textBlockItem->textItem()->boundingRect();
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
                
                // 将当前页的文本项列表添加到全局列表
                m_pageTextItems.append(pageTextItems);
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
    if (!block)
        return;
    auto it = m_blockItems.find(block);
    if (it != m_blockItems.end() && it.value()) {
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
    
    // 找到鼠标位置所在的 TextBlockItem
    TextBlockItem *targetBlockItem = nullptr;
    int blockIndex = 0;
    
    for (int i = 0; i < page->blockCount(); ++i) {
        Block *block = page->block(i);
        auto it = m_blockItems.find(block);
        if (it != m_blockItems.end()) {
            TextBlockItem *textBlockItem = dynamic_cast<TextBlockItem*>(it.value());
            if (textBlockItem) {
                // 检查场景坐标是否在该块的边界内
                QPointF blockScenePos = textBlockItem->scenePos();
                QRectF blockRect = textBlockItem->boundingRect();
                QRectF blockSceneRect(blockScenePos.x(), blockScenePos.y(), 
                                       blockRect.width(), blockRect.height());
                
                if (scenePos.y() <= blockSceneRect.bottom()) {
                    targetBlockItem = textBlockItem;
                    blockIndex = i;
                    break;
                }
            }
        }
    }
    
    // 如果没有找到（鼠标在所有块下方），使用最后一个块
    if (!targetBlockItem && page->blockCount() > 0) {
        for (int i = page->blockCount() - 1; i >= 0; --i) {
            Block *block = page->block(i);
            auto it = m_blockItems.find(block);
            if (it != m_blockItems.end()) {
                TextBlockItem *textBlockItem = dynamic_cast<TextBlockItem*>(it.value());
                if (textBlockItem) {
                    targetBlockItem = textBlockItem;
                    blockIndex = i;
                    break;
                }
            }
        }
    }
    
    pos.blockIndex = blockIndex;
    
    if (targetBlockItem) {
        // 将场景坐标转换为 TextBlockItem 的局部坐标
        QPointF localPos = targetBlockItem->mapFromScene(scenePos);
        
        // 使用 TextBlockItem 的 hitTest 方法
        pos.offset = targetBlockItem->hitTest(localPos);
    }
    
    return pos;
}

QPointF DocumentScene::calculateCursorVisualPosition(const CursorPosition &pos) const
{
    QPointF result(0, 0);
    
    if (!m_document) {
        return result;
    }
    
    // 找到所在的页（简单假设：第0个section）
    Section *section = m_document->section(0);
    if (!section) {
        return result;
    }
    
    // 找到对应页的index：这里简化，假设每个section只有1页
    int pageIndex = 0;
    if (pageIndex < 0 || pageIndex >= section->pageCount()) {
        return result;
    }
    
    Page *page = section->page(pageIndex);
    if (!page) {
        return result;
    }
    
    // 获取 TextBlockItem
    TextBlockItem *textBlockItem = nullptr;
    if (pos.blockIndex >= 0 && pos.blockIndex < page->blockCount()) {
        Block *block = page->block(pos.blockIndex);
        auto it = m_blockItems.find(block);
        if (it != m_blockItems.end()) {
            textBlockItem = dynamic_cast<TextBlockItem*>(it.value());
        }
    }
    
    if (!textBlockItem) {
        return result;
    }
    
    // 使用 TextBlockItem 的 cursorPositionAt 方法
    TextBlockItem::CursorVisualInfo visualInfo = textBlockItem->cursorPositionAt(pos.offset);
    
    // 获取 TextBlockItem 在场景中的位置
    QPointF blockScenePos = textBlockItem->scenePos();
    
    // 计算最终的场景坐标
    result.setX(blockScenePos.x() + visualInfo.position.x());
    result.setY(blockScenePos.y() + visualInfo.position.y());
    
    // 更新光标高度
    if (m_cursorItem) {
        m_cursorItem->setPosition(result, visualInfo.height);
    }
    
    return result;
}

// ========== 新增方法：根据 CursorPosition 直接更新光标 ==========
void DocumentScene::updateCursorFromPosition(const CursorPosition &pos)
{
    QPointF visualPos = calculateCursorVisualPosition(pos);
    
    // 需要找到对应的 TextBlockItem 来获取光标高度
    qreal cursorHeight = 20.0;
    
    if (m_document && pos.blockIndex >= 0) {
        // 找到所在的页（简单假设：第0个section）
        Section *section = m_document->section(0);
        if (section) {
            int pageIndex = 0;
            if (pageIndex >= 0 && pageIndex < section->pageCount()) {
                Page *page = section->page(pageIndex);
                if (page && pos.blockIndex >= 0 && pos.blockIndex < page->blockCount()) {
                    Block *block = page->block(pos.blockIndex);
                    auto it = m_blockItems.find(block);
                    if (it != m_blockItems.end()) {
                        TextBlockItem *textBlockItem = dynamic_cast<TextBlockItem*>(it.value());
                        if (textBlockItem) {
                            TextBlockItem::CursorVisualInfo visualInfo = textBlockItem->cursorPositionAt(pos.offset);
                            cursorHeight = visualInfo.height;
                        }
                    }
                }
            }
        }
    }
    
    updateCursor(visualPos, cursorHeight);
}

QList<QRectF> DocumentScene::calculateSelectionRects(const SelectionRange &range) const
{
    QList<QRectF> rects;
    
    if (!m_document) {
        return rects;
    }
    
    // 归一化选择范围
    SelectionRange normalizedRange = range;
    normalizedRange.normalize();
    
  //  QDebug() << "DocumentScene::calculateSelectionRects - 计算选择矩形，范围:"
             // << "起始块:" << normalizedRange.startBlock
             // << "起始偏移:" << normalizedRange.startOffset
             // << "结束块:" << normalizedRange.endBlock
             // << "结束偏移:" << normalizedRange.endOffset;
    
    // 简单假设只有一个section和一个page
    Section *section = m_document->section(0);
    if (!section) {
        return rects;
    }
    
    int pageIndex = 0;
    
    // 遍历选择范围内的所有块
    for (int blockIdx = normalizedRange.startBlock; blockIdx <= normalizedRange.endBlock; ++blockIdx) {
        // 检查是否超出范围
        if (blockIdx < 0 || blockIdx >= section->blockCount()) {
            continue;
        }
        
        // 获取对应的QGraphicsTextItem
        if (pageIndex >= 0 && pageIndex < m_pageTextItems.size() &&
            blockIdx >= 0 && blockIdx < m_pageTextItems[pageIndex].size()) {
            
            QGraphicsTextItem *textItem = m_pageTextItems[pageIndex][blockIdx];
            if (!textItem) {
                continue;
            }
            
            QTextDocument *doc = textItem->document();
            if (!doc) {
                continue;
            }
            
            QString text = doc->toPlainText();
          //  QDebug() << "  块" << blockIdx << "文本:" << text;
            
            // 确定当前块的选择起始和结束偏移
            int startOffset = 0;
            int endOffset = text.length();
            
            if (blockIdx == normalizedRange.startBlock) {
                startOffset = qMin(normalizedRange.startOffset, text.length());
            }
            if (blockIdx == normalizedRange.endBlock) {
                endOffset = qMin(normalizedRange.endOffset, text.length());
            }
            
          //  QDebug() << "    起始偏移:" << startOffset << "结束偏移:" << endOffset;
            
            // 如果起始和结束相同，跳过
            if (startOffset >= endOffset) {
                continue;
            }
            
            // 使用QTextCursor获取选择的字符范围
            QTextCursor cursor(doc);
            cursor.setPosition(startOffset);
            cursor.setPosition(endOffset, QTextCursor::KeepAnchor);
            
            // 获取选择区域
            QTextBlock block = cursor.block();
            
            // 遍历选择范围内的所有行
            QTextLayout *layout = block.layout();
            if (!layout) {
                continue;
            }
            
            for (int i = 0; i < layout->lineCount(); ++i) {
                QTextLine line = layout->lineAt(i);
                int lineStart = line.textStart();
                int lineEnd = line.textStart() + line.textLength();
                
                // 确定当前行与选择范围的重叠
                int selStart = qMax(startOffset, lineStart);
                int selEnd = qMin(endOffset, lineEnd);
                
                if (selStart < selEnd) {
                    // 计算选择在该行的起始和结束位置
                    qreal x1 = line.cursorToX(selStart);
                    qreal x2 = line.cursorToX(selEnd);
                    
                    // 获取textItem在场景中的位置
                    QPointF itemScenePos = textItem->scenePos();
                    
                    // 创建选择矩形
                    QRectF rect(
                        itemScenePos.x() + qMin(x1, x2),
                        itemScenePos.y() + line.y(),
                        qAbs(x2 - x1),
                        line.height()
                    );
                    
                    rects.append(rect);
                  //  QDebug() << "    添加矩形:" << rect;
                }
            }
        }
    }
    
    return rects;
}

} // namespace QtWordEditor
