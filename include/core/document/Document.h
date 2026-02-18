#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <QObject>
#include <QList>
#include <QString>
#include <QDateTime>
#include <QUndoStack>
#include "core/Global.h"

namespace QtWordEditor {

class Section;
class Block;

/**
 * @brief 文档类是整个文档的根容器
 *
 * 负责管理文档的多个节（Section）、元数据信息以及撤销重做栈。
 * 这是文档对象模型的核心类，提供对整个文档结构的访问和操作接口。
 */
class Document : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief 构造函数
     * @param parent 父对象指针，默认为nullptr
     */
    explicit Document(QObject *parent = nullptr);
    
    /**
     * @brief 析构函数
     * 自动清理所有节对象
     */
    ~Document() override;

    // ========== 文档标识相关方法 ==========
    
    /**
     * @brief 获取文档ID
     * @return 文档唯一标识符
     */
    int documentId() const;
    
    /**
     * @brief 设置文档ID
     * @param id 新的文档ID
     */
    void setDocumentId(int id);

    // ========== 元数据相关方法 ==========
    
    /**
     * @brief 获取文档标题
     * @return 文档标题字符串
     */
    QString title() const;
    
    /**
     * @brief 设置文档标题
     * @param title 新的文档标题
     */
    void setTitle(const QString &title);

    /**
     * @brief 获取文档作者
     * @return 作者姓名字符串
     */
    QString author() const;
    
    /**
     * @brief 设置文档作者
     * @param author 新的作者姓名
     */
    void setAuthor(const QString &author);

    /**
     * @brief 获取文档创建时间
     * @return 创建时间戳
     */
    QDateTime created() const;
    
    /**
     * @brief 获取文档修改时间
     * @return 最后修改时间戳
     */
    QDateTime modified() const;
    
    /**
     * @brief 设置文档修改时间
     * @param modified 新的修改时间戳
     */
    void setModified(const QDateTime &modified);

    // ========== 节（Section）管理方法 ==========
    
    /**
     * @brief 获取文档中节的数量
     * @return 节的总数
     */
    int sectionCount() const;
    
    /**
     * @brief 根据索引获取指定节
     * @param index 节的索引位置
     * @return 指向节的指针，如果索引无效则返回nullptr
     */
    Section *section(int index) const;
    
    /**
     * @brief 在文档末尾添加节
     * @param section 要添加的节对象指针
     */
    void addSection(Section *section);
    
    /**
     * @brief 在指定位置插入节
     * @param index 插入位置索引
     * @param section 要插入的节对象指针
     */
    void insertSection(int index, Section *section);
    
    /**
     * @brief 移除指定索引的节
     * @param index 要移除的节索引
     */
    void removeSection(int index);

    // ========== 块（Block）全局访问方法 ==========
    
    /**
     * @brief 获取文档中所有块的总数
     * @return 所有节中块的总数量
     */
    int blockCount() const;
    
    /**
     * @brief 根据全局索引获取块
     * @param globalIndex 全局块索引（跨所有节）
     * @return 指向块的指针，如果索引无效则返回nullptr
     */
    Block *block(int globalIndex) const;

    // ========== 撤销重做栈相关方法 ==========
    
    /**
     * @brief 获取文档的撤销栈
     * @return 指向QUndoStack的指针
     */
    QUndoStack *undoStack() const;

    // ========== 导出方法（主要用于测试）==========
    
    /**
     * @brief 将文档导出为纯文本格式
     * @return 文档的纯文本表示（主要用于测试目的）
     */
    QString plainText() const;

signals:
    /** @brief 文档内容发生变化时发出的信号 */
    void documentChanged();
    
    /** @brief 添加节时发出的信号 */
    void sectionAdded(int index);
    
    /** @brief 移除节时发出的信号 */
    void sectionRemoved(int index);
    
    /** @brief 添加块时发出的信号 */
    void blockAdded(int globalIndex);
    
    /** @brief 移除块时发出的信号 */
    void blockRemoved(int globalIndex);
    
    /** @brief 布局发生变化时发出的信号 */
    void layoutChanged();

private:
    /**
     * @brief 更新文档中所有块的全局位置
     * 当节结构发生变化时调用此方法重新计算块的位置
     */
    void updateBlockPositions();

private:
    int m_documentId = -1;          ///< 文档唯一标识符
    QString m_title;                ///< 文档标题
    QString m_author;               ///< 文档作者
    QDateTime m_created;            ///< 文档创建时间
    QDateTime m_modified;           ///< 文档最后修改时间
    QList<Section*> m_sections;     ///< 文档包含的所有节列表
    QScopedPointer<QUndoStack> m_undoStack; ///< 撤销重做栈
};

} // namespace QtWordEditor

#endif // DOCUMENT_H