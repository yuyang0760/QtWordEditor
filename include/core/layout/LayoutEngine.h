#ifndef LAYOUTENGINE_H
#define LAYOUTENGINE_H

#include <QObject>
#include <QHash>
#include "core/Global.h"

namespace QtWordEditor {

class Document;
class Block;

/**
 * @brief 布局引擎类，负责文档内容的分页和块的位置大小计算
 *
 * 该类是文档渲染系统的核心组件，负责：
 * 1. 根据页面设置对文档内容进行分页
 * 2. 计算每个块在页面上的精确位置和尺寸
 * 3. 处理增量布局以提高性能
 * 4. 缓存块高度信息以避免重复计算
 */
class LayoutEngine : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief 构造函数
     * @param parent 父对象指针，默认为nullptr
     */
    explicit LayoutEngine(QObject *parent = nullptr);
    
    /**
     * @brief 析构函数
     */
    ~LayoutEngine() override;

    /**
     * @brief 设置要布局的文档
     * @param document 目标文档指针
     */
    void setDocument(Document *document);
    
    /**
     * @brief 获取当前布局的文档
     * @return 当前文档指针
     */
    Document *document() const;

    // ========== 页面设置相关方法 ==========
    
    /**
     * @brief 设置页面尺寸和边距
     * @param width 页面宽度（点为单位）
     * @param height 页面高度（点为单位）
     * @param margin 页面边距（点为单位）
     */
    void setPageSize(qreal width, qreal height, qreal margin);
    
    /**
     * @brief 获取当前页面设置
     * @param width 输出参数，返回页面宽度
     * @param height 输出参数，返回页面高度
     * @param margin 输出参数，返回页面边距
     */
    void getPageSize(qreal *width, qreal *height, qreal *margin) const;

    // ========== 布局执行方法 ==========
    
    /**
     * @brief 执行完整的文档布局
     * 对整个文档重新进行布局计算
     */
    void layout();

    /**
     * @brief 从指定块索引开始执行增量布局
     * @param blockIndex 开始布局的块索引
     * 用于优化性能，只重新布局受影响的部分
     */
    void layoutFrom(int blockIndex);

    /**
     * @brief 计算指定块的高度
     * @param block 要计算的块
     * @param maxWidth 最大可用宽度
     * @return 计算得到的块高度
     */
    qreal calculateBlockHeight(Block *block, qreal maxWidth);

signals:
    /** @brief 布局发生变化时发出的信号 */
    void layoutChanged();

private:
    Document *m_document = nullptr;     ///< 当前布局的文档
    qreal m_pageWidth = 595.0;          ///< 页面宽度（A4纸宽，单位：点）
    qreal m_pageHeight = 842.0;         ///< 页面高度（A4纸高，单位：点）
    qreal m_margin = 72.0;              ///< 页面边距（1英寸，单位：点）
    QHash<Block*, qreal> m_cachedHeights; ///< 块高度缓存，避免重复计算
};

} // namespace QtWordEditor

#endif // LAYOUTENGINE_H