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
                        
                        // 设置位置
                        qreal textX = Constants::PAGE_MARGIN;
                        qreal textY = Constants::PAGE_MARGIN + blockIdx * 30;
                        textBlockItem->setPos(textX, textY);
                        
                        // 添加到场景
                        addItem(textBlockItem);
                        
                        // 添加到 m_blockItems 和 m_pageTextItems
                        m_blockItems.insert(block, textBlockItem);
                        pageTextItems.append(textBlockItem->textItem());
                        
                      //  QDebug() << ">>>>>>>>>>      文本块项位置:" << textX << "," << textY;
                      //  QDebug() << ">>>>>>>>>>      文本项边界矩形:" << textBlockItem->textItem()->boundingRect();
                    }
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
}

void DocumentScene::updateSingleTextItem(Block *block)
{
    if (!block)
        return;
    auto it = m_blockItems.find(block);
    if (it != m_blockItems.end() && it.value()) {
        it.value()->updateBlock();
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
    
    // 使用真实的 QGraphicsTextItem
    if (pageIndex >= 0 && pageIndex < m_pageTextItems.size() &&
        blockIndex >= 0 && blockIndex < m_pageTextItems[pageIndex].size()) {
        QGraphicsTextItem *textItem = m_pageTextItems[pageIndex][blockIndex];
        if (textItem) {
            // 使用 mapFromScene 转换坐标到 textItem 的局部坐标
            QPointF localPos = textItem->mapFromScene(scenePos);
            
            // 使用 hitTest 获取准确位置
            pos.offset = textItem->document()->documentLayout()->hitTest(localPos, Qt::FuzzyHit);
        }
    }
    
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
    
    // 获取真实的 QGraphicsTextItem
    QGraphicsTextItem *textItem = nullptr;
    if (pageIndex >= 0 && pageIndex < m_pageTextItems.size() &&
        pos.blockIndex >= 0 && pos.blockIndex < m_pageTextItems[pageIndex].size()) {
        textItem = m_pageTextItems[pageIndex][pos.blockIndex];
    }
    
    if (!textItem) {
      //  QDebug() << "  文本项指针为空！页面索引:" << pageIndex 
      //           << "页面文本项列表大小:" << m_pageTextItems.size()
      //           << "块索引:" << pos.blockIndex;
        if (pageIndex < m_pageTextItems.size()) {
          //  QDebug() << "  当前页面文本项数量:" << m_pageTextItems[pageIndex].size();
        }
        return result;
    }
    
    QTextDocument *doc = textItem->document();
    if (!doc) {
      //  QDebug() << "  文档指针为空！";
        return result;
    }
    
    QString text = doc->toPlainText();
    int charOffset = qMin(pos.offset, text.length());
    
  //  QDebug() << "  文本内容:" << text << "字符偏移:" << charOffset;
    
    // 使用 QTextCursor 和文档布局来计算光标位置
    QTextCursor cursor(doc);
    cursor.setPosition(charOffset);
    
    QTextBlock block = cursor.block();
    QTextLayout *layout = block.layout();
    
    if (!layout) {
      //  QDebug() << "  布局指针为空！";
        return result;
    }
    
    int positionInBlock = cursor.positionInBlock();
    
    qreal x = 0;
    qreal y = 0;
    
    if (layout->lineCount() > 0) {
        // 找到光标所在的行
        for (int i = 0; i < layout->lineCount(); ++i) {
            QTextLine line = layout->lineAt(i);
            if (positionInBlock >= line.textStart() && positionInBlock <= line.textStart() + line.textLength()) {
                y = line.y();
                x = line.cursorToX(positionInBlock);
                break;
            }
        }
    }
    
  //  QDebug() << "  X坐标:" << x << "Y坐标:" << y;
    
    // 获取 textItem 在场景中的位置（注意：它是 TextBlockItem 的子项！）
    QPointF itemScenePos = textItem->scenePos();
  //  QDebug() << "  文本项场景坐标:" << itemScenePos;
    
    result.setX(itemScenePos.x() + x);
    result.setY(itemScenePos.y() + y);
    
  //  QDebug() << "  返回结果坐标:" << result;
    
    return result;
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
