#ifndef STYLEMANAGER_H
#define STYLEMANAGER_H

#include <QObject>
#include <QHash>
#include <QString>
#include "core/document/CharacterStyle.h"
#include "core/document/ParagraphStyle.h"
#include "core/Global.h"

namespace QtWordEditor {

class Document;

/**
 * @brief 字符样式信息结构体
 * 包含样式本身和父样式名称
 */
struct CharacterStyleInfo {
    CharacterStyle style;          ///< 字符样式
    QString parentStyleName;        ///< 父样式名称（可选）
};

/**
 * @brief 段落样式信息结构体
 * 包含样式本身和父样式名称
 */
struct ParagraphStyleInfo {
    ParagraphStyle style;           ///< 段落样式
    QString parentStyleName;        ///< 父样式名称（可选）
};

/**
 * @brief 样式管理器类，管理命名的字符样式和段落样式
 *
 * 该类提供了样式模板的存储、检索和应用功能，支持：
 * 1. 字符样式的管理（字体、字号、颜色等）
 * 2. 段落样式的管理（对齐、缩进、行距等）
 * 3. 样式到文档内容的应用
 * 4. 样式模板的维护和复用
 * 5. 样式继承机制
 */
class StyleManager : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief 构造函数
     * @param parent 父对象指针，默认为nullptr
     */
    explicit StyleManager(QObject *parent = nullptr);
    
    /**
     * @brief 析构函数
     */
    ~StyleManager() override;

    /**
     * @brief 设置关联的文档
     * @param document 目标文档指针
     */
    void setDocument(Document *document);
    
    /**
     * @brief 获取关联的文档
     * @return 当前文档指针
     */
    Document *document() const;

    // ========== 字符样式管理方法 ==========
    
    /**
     * @brief 添加字符样式
     * @param name 样式名称
     * @param style 字符样式对象
     * @param parentStyleName 父样式名称（可选）
     */
    void addCharacterStyle(const QString &name, const CharacterStyle &style, const QString &parentStyleName = QString());
    
    /**
     * @brief 根据名称获取字符样式（不包含继承）
     * @param name 样式名称
     * @return 对应的字符样式对象
     */
    CharacterStyle characterStyle(const QString &name) const;
    
    /**
     * @brief 获取解析继承后的字符样式
     * @param name 样式名称
     * @return 解析继承后的完整字符样式
     */
    CharacterStyle getResolvedCharacterStyle(const QString &name) const;
    
    /**
     * @brief 检查是否存在指定名称的字符样式
     * @param name 样式名称
     * @return 如果存在返回true，否则返回false
     */
    bool hasCharacterStyle(const QString &name) const;
    
    /**
     * @brief 获取所有字符样式名称列表
     * @return 样式名称字符串列表
     */
    QStringList characterStyleNames() const;
    
    /**
     * @brief 设置字符样式的父样式
     * @param styleName 样式名称
     * @param parentStyleName 父样式名称
     */
    void setCharacterStyleParent(const QString &styleName, const QString &parentStyleName);
    
    /**
     * @brief 获取字符样式的父样式名称
     * @param styleName 样式名称
     * @return 父样式名称，如果没有则返回空字符串
     */
    QString characterStyleParent(const QString &styleName) const;

    // ========== 段落样式管理方法 ==========
    
    /**
     * @brief 添加段落样式
     * @param name 样式名称
     * @param style 段落样式对象
     * @param parentStyleName 父样式名称（可选）
     */
    void addParagraphStyle(const QString &name, const ParagraphStyle &style, const QString &parentStyleName = QString());
    
    /**
     * @brief 根据名称获取段落样式（不包含继承）
     * @param name 样式名称
     * @return 对应的段落样式对象
     */
    ParagraphStyle paragraphStyle(const QString &name) const;
    
    /**
     * @brief 获取解析继承后的段落样式
     * @param name 样式名称
     * @return 解析继承后的完整段落样式
     */
    ParagraphStyle getResolvedParagraphStyle(const QString &name) const;
    
    /**
     * @brief 检查是否存在指定名称的段落样式
     * @param name 样式名称
     * @return 如果存在返回true，否则返回false
     */
    bool hasParagraphStyle(const QString &name) const;
    
    /**
     * @brief 获取所有段落样式名称列表
     * @return 样式名称字符串列表
     */
    QStringList paragraphStyleNames() const;
    
    /**
     * @brief 设置段落样式的父样式
     * @param styleName 样式名称
     * @param parentStyleName 父样式名称
     */
    void setParagraphStyleParent(const QString &styleName, const QString &parentStyleName);
    
    /**
     * @brief 获取段落样式的父样式名称
     * @param styleName 样式名称
     * @return 父样式名称，如果没有则返回空字符串
     */
    QString paragraphStyleParent(const QString &styleName) const;

    // ========== 样式应用方法 ==========
    
    /**
     * @brief 应用字符样式到指定文本范围
     * @param styleName 样式名称
     * @param blockIndex 块索引
     * @param start 起始偏移量
     * @param end 结束偏移量
     */
    void applyCharacterStyle(const QString &styleName, int blockIndex, int start, int end);
    
    /**
     * @brief 应用段落样式到指定块列表
     * @param styleName 样式名称
     * @param blockIndices 块索引列表
     */
    void applyParagraphStyle(const QString &styleName, const QList<int> &blockIndices);

signals:
    /** @brief 样式发生改变时发出的信号 */
    void stylesChanged();
    /** @brief 字符样式发生改变时发出的信号 */
    void characterStyleChanged(const QString &styleName);
    /** @brief 段落样式发生改变时发出的信号 */
    void paragraphStyleChanged(const QString &styleName);

private:
    /**
     * @brief 初始化默认字符样式
     */
    void initializeDefaultCharacterStyles();
    
    /**
     * @brief 初始化默认段落样式
     */
    void initializeDefaultParagraphStyles();
    
    Document *m_document = nullptr;                             ///< 关联的文档
    QHash<QString, CharacterStyleInfo> m_characterStyles;      ///< 字符样式哈希表
    QHash<QString, ParagraphStyleInfo> m_paragraphStyles;      ///< 段落样式哈希表
};

} // namespace QtWordEditor

#endif // STYLEMANAGER_H