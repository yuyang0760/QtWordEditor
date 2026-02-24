/**
 * @file Section.h
 * @brief 文档节类
 * 
 * 表示文档中的一个逻辑节（如章节），包含块列表和由布局引擎生成的页面。
 * 是文档结构的中间层，介于文档和块之间。
 */

#ifndef SECTION_H
#define SECTION_H

#include <QObject>
#include <QList>
#include <QString>
#include "core/Global.h"

namespace QtWordEditor {

class Block;
class Page;

/**
 * @brief 文档节类
 * 
 * 表示文档中的一个逻辑节（如章节），包含块列表和由布局引擎生成的页面。
 * 是文档结构的中间层，介于文档和块之间。
 */
class Section : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief 构造函数
     * @param parent 父对象指针，默认为nullptr
     */
    explicit Section(QObject *parent = nullptr);
    
    /**
     * @brief 析构函数
     */
    ~Section() override;

    // ========== 节信息 ==========
    
    /**
     * @brief 获取节编号
     * @return 节编号
     */
    int sectionNumber() const;
    
    /**
     * @brief 设置节编号
     * @param number 新的节编号
     */
    void setSectionNumber(int number);

    // ========== 页眉页脚 ==========
    
    /**
     * @brief 获取页眉内容
     * @return 页眉文本
     */
    QString header() const;
    
    /**
     * @brief 设置页眉内容
     * @param header 新的页眉文本
     */
    void setHeader(const QString &header);

    /**
     * @brief 获取页脚内容
     * @return 页脚文本
     */
    QString footer() const;
    
    /**
     * @brief 设置页脚内容
     * @param footer 新的页脚文本
     */
    void setFooter(const QString &footer);

    // ========== 块管理 ==========
    
    /**
     * @brief 获取块数量
     * @return 块总数
     */
    int blockCount() const;
    
    /**
     * @brief 获取指定索引的块
     * @param index 块索引
     * @return 块指针
     */
    Block *block(int index) const;
    
    /**
     * @brief 添加块到末尾
     * @param block 要添加的块
     */
    void addBlock(Block *block);
    
    /**
     * @brief 在指定位置插入块
     * @param index 插入位置
     * @param block 要插入的块
     */
    void insertBlock(int index, Block *block);
    
    /**
     * @brief 移除指定位置的块
     * @param index 要移除的块索引
     */
    void removeBlock(int index);

    // ========== 页面管理（运行时） ==========
    
    /**
     * @brief 获取页面数量
     * @return 页面总数
     */
    int pageCount() const;
    
    /**
     * @brief 获取指定索引的页面
     * @param index 页面索引
     * @return 页面指针
     */
    Page *page(int index) const;
    
    /**
     * @brief 添加页面
     * @param page 要添加的页面
     */
    void addPage(Page *page);
    
    /**
     * @brief 清除所有页面
     */
    void clearPages();

signals:
    /** @brief 添加块时发出的信号 */
    void blockAdded(int index);
    
    /** @brief 移除块时发出的信号 */
    void blockRemoved(int index);
    
    /** @brief 页面列表变化时发出的信号 */
    void pagesChanged();

private:
    int m_sectionNumber = 0; ///< 节编号
    QString m_header;        ///< 页眉内容
    QString m_footer;        ///< 页脚内容
    QList<Block*> m_blocks;  ///< 块列表
    QList<Page*> m_pages;    ///< 页面列表（由节拥有）
};

} // namespace QtWordEditor

#endif // SECTION_H