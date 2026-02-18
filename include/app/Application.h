#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>
#include <QScopedPointer>
#include <QStringList>
#include <QTranslator>

class QSettings;

namespace QtWordEditor {

class ApplicationPrivate;

/**
 * @brief 应用程序类，管理全局应用程序实例
 *
 * 该类继承自QApplication，负责：
 * 1. 初始化应用程序各项组件
 * 2. 加载和保存应用程序设置
 * 3. 管理多语言翻译
 * 4. 维护最近使用的文件列表
 * 5. 处理全局异常和应用程序生命周期
 */
class Application : public QApplication
{
    Q_OBJECT
public:
    /**
     * @brief 构造函数
     * @param argc 命令行参数数量
     * @param argv 命令行参数数组
     */
    Application(int &argc, char **argv);
    
    /**
     * @brief 析构函数
     */
    ~Application() override;

    /**
     * @brief 初始化应用程序组件
     * @return 初始化成功返回true，失败返回false
     */
    bool init();

    /**
     * @brief 从持久化存储加载应用程序设置
     */
    void loadSettings();

    /**
     * @brief 保存应用程序设置到持久化存储
     */
    void saveSettings();

    /**
     * @brief 获取全局应用程序实例
     * @return 指向应用程序实例的指针
     */
    static Application *instance();

    /**
     * @brief 添加文件到最近使用文件列表
     * @param filePath 文件路径
     */
    void addRecentFile(const QString &filePath);

    /**
     * @brief 获取最近使用文件列表
     * @return 文件路径字符串列表
     */
    QStringList recentFiles() const;

    /**
     * @brief 清空最近使用文件列表
     */
    void clearRecentFiles();

    /**
     * @brief 设置要保留的最近文件最大数量
     * @param max 最大文件数量
     */
    void setMaxRecentFiles(int max);

    /**
     * @brief 获取最近文件列表的最大容量
     * @return 最大文件数量
     */
    int maxRecentFiles() const;
    
    /**
     * @brief 加载应用程序翻译文件
     */
    void loadTranslations();
    
    /**
     * @brief 切换到不同的语言
     * @param language 语言代码（如"zh_CN", "en_US"）
     */
    void switchLanguage(const QString &language);

private:
    QScopedPointer<ApplicationPrivate> d_ptr;  ///< 私有实现指针
    Q_DECLARE_PRIVATE(Application)

    QStringList m_recentFiles;        ///< 最近使用的文件列表
    int m_maxRecentFiles = 10;        ///< 最近文件列表最大容量
    QTranslator *m_translator = nullptr; ///< 当前使用的翻译器
};

} // namespace QtWordEditor

#endif // APPLICATION_H