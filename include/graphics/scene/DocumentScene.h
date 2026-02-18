#ifndef DOCUMENTSCENE_H
#define DOCUMENTSCENE_H

#include <QGraphicsScene>
#include <QHash>
#include <QList>
#include "core/Global.h"

namespace QtWordEditor {

class Document;
class Block;
class Page;
class BaseBlockItem;
class CursorItem;
class SelectionItem;
class PageItem;
struct CursorPosition;
struct SelectionRange;

/**
 * @brief 文档场景类，管理文档的图形表示
 *
 * 该类继承自QGraphicsScene，负责：
 * 1. 将文档内容转换为可视化的图形项
 * 2. 管理页面、块、光标、选择区域等图形元素
 * 3. 处理文档变化时的场景更新
 * 4. 提供场景坐标与文档坐标的转换功能
 */
class DocumentScene : public QGraphicsScene
{
    Q_OBJECT
public:
    /**
     * @brief 构造函数
     * @param parent 父对象指针，默认为nullptr
     */
    explicit DocumentScene(QObject *parent = nullptr);
    
    /**
     * @brief 析构函数
     */
    ~DocumentScene() override;

    /**
     * @brief 设置要显示的文档
     * @param document 目标文档指针
     */
    void setDocument(Document *document);
    
    /**
     * @brief 获取当前显示的文档
     * @return 当前文档指针
     */
    Document *document() const;

    // ========== 场景重建方法 ==========
    
    /**
     * @brief 根据文档内容重新构建整个场景
     * 清除现有场景并重新创建所有图形项
     */
    void rebuildFromDocument();

    /**
     * @brief 更新所有文本项的内容而不重建场景
     * 用于内容变化但布局未改变的情况
     */
    void updateAllTextItems();
    
    /**
     * @brief 只更新单个文本块
     * @param block 要更新的块
     */
    void updateSingleTextItem(Block *block);

    // ========== 光标相关方法 ==========
    
    /**
     * @brief 更新光标位置（世界坐标）
     * @param pos 光标位置坐标
     * @param height 光标高度
     */
    void updateCursor(const QPointF &pos, qreal height);

    // ========== 选择区域相关方法 ==========
    
    /**
     * @brief 更新选择区域矩形
     * @param rects 选择区域矩形列表
     */
    void updateSelection(const QList<QRectF> &rects);
    
    /**
     * @brief 清除选择区域
     */
    void clearSelection();

    /**
     * @brief 根据选择范围计算选择区域矩形列表
     * @param range 选择范围
     * @return 选择区域矩形列表
     */
    QList<QRectF> calculateSelectionRects(const SelectionRange &range) const;

    // ========== 页面管理方法 ==========
    
    /**
     * @brief 清除所有页面
     */
    void clearPages();
    
    /**
     * @brief 添加页面
     * @param page 要添加的页面对象
     */
    void addPage(Page *page);
    
    /**
     * @brief 获取指定位置的页面
     * @param scenePos 场景坐标位置
     * @return 对应的页面对象指针，如果不存在则返回nullptr
     */
    Page *pageAt(const QPointF &scenePos) const;
    
    // ========== 坐标转换方法 ==========
    
    /**
     * @brief 根据场景位置获取光标位置
     * @param scenePos 场景坐标位置
     * @return 对应的光标位置结构体
     */
    CursorPosition cursorPositionAt(const QPointF &scenePos) const;
    
    /**
     * @brief 根据光标位置计算视觉位置
     * @param pos 光标位置结构体
     * @return 对应的场景坐标位置
     */
    QPointF calculateCursorVisualPosition(const CursorPosition &pos) const;

public slots:
    /**
     * @brief 处理块添加事件
     * @param globalIndex 添加的块的全局索引
     */
    void onBlockAdded(int globalIndex);
    
    /**
     * @brief 处理块移除事件
     * @param globalIndex 移除的块的全局索引
     */
    void onBlockRemoved(int globalIndex);
    
    /**
     * @brief 处理布局变化事件
     */
    void onLayoutChanged();

private:
    Document *m_document;                                   ///< 关联的文档
    QHash<Block*, BaseBlockItem*> m_blockItems;            ///< 块到图形项的映射
    QList<PageItem*> m_pageItems;                          ///< 页面项列表
    QVector<QVector<QGraphicsTextItem*>> m_pageTextItems;  ///< 存储每个页、块对应的 QGraphicsTextItem
    CursorItem *m_cursorItem;                              ///< 光标图形项
    SelectionItem *m_selectionItem;                        ///< 选择区域图形项
};

} // namespace QtWordEditor

#endif // DOCUMENTSCENE_H