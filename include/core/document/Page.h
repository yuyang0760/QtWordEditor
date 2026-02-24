/**
 * @file Page.h
 * @brief 页面类
 * 
 * 表示由布局引擎生成的物理页面，包含对出现在此页面上的块的引用（非拥有）。
 * 用于文档的分页显示和布局管理。
 */

#ifndef PAGE_H
#define PAGE_H

#include <QList>
#include <QRectF>
#include "core/Global.h"

namespace QtWordEditor {

class Block;

/**
 * @brief 页面类
 * 
 * 表示由布局引擎生成的物理页面，包含对出现在此页面上的块的引用（非拥有）。
 * 用于文档的分页显示和布局管理。
 */
class Page
{
public:
    /**
     * @brief 构造函数
     * @param pageNumber 页码
     * @param pageRect 页面矩形（包含边距）
     * @param contentRect 内容矩形（不含边距）
     */
    Page(int pageNumber, const QRectF &pageRect, const QRectF &contentRect);
    
    /**
     * @brief 析构函数
     */
    ~Page();

    // ========== 页面信息 ==========
    
    /**
     * @brief 获取页码
     * @return 页码
     */
    int pageNumber() const;
    
    /**
     * @brief 设置页码
     * @param number 新的页码
     */
    void setPageNumber(int number);
    
    /**
     * @brief 获取页面矩形
     * @return 页面矩形（包含边距）
     */
    QRectF pageRect() const;
    
    /**
     * @brief 获取内容矩形
     * @return 内容矩形（不含边距）
     */
    QRectF contentRect() const;

    // ========== 块管理 ==========
    
    /**
     * @brief 获取块数量
     * @return 块总数
     */
    int blockCount() const;
    
    /**
     * @brief 获取指定索引的块
     * @param index 块索引
     * @return 块指针（非拥有）
     */
    Block *block(int index) const;
    
    /**
     * @brief 添加块引用
     * @param block 要添加的块指针
     */
    void addBlock(Block *block);
    
    /**
     * @brief 清除所有块引用
     */
    void clearBlocks();
    
    /**
     * @brief 判断页面是否为空
     * @return 如果页面为空返回true
     */
    bool isEmpty() const;

private:
    int m_pageNumber;      ///< 页码
    QRectF m_pageRect;      ///< 页面矩形（包含边距）
    QRectF m_contentRect;   ///< 内容矩形（不含边距）
    QList<Block*> m_blocks; ///< 块引用列表（非拥有）
};

} // namespace QtWordEditor

#endif // PAGE_H