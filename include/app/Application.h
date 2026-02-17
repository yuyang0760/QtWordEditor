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
 * @brief The Application class manages the global application instance.
 *
 * It initializes application settings, loads translations, and handles global exceptions.
 */
class Application : public QApplication
{
    Q_OBJECT
public:
    Application(int &argc, char **argv);
    ~Application() override;

    /**
     * @brief Initialize application components.
     * @return true if successful; false otherwise.
     */
    bool init();

    /**
     * @brief Load application settings from persistent storage.
     */
    void loadSettings();

    /**
     * @brief Save application settings to persistent storage.
     */
    void saveSettings();

    /**
     * @brief Returns the global application instance.
     */
    static Application *instance();

    /**
     * @brief Add a file to the recent files list.
     */
    void addRecentFile(const QString &filePath);

    /**
     * @brief Get the list of recent files.
     */
    QStringList recentFiles() const;

    /**
     * @brief Clear the recent files list.
     */
    void clearRecentFiles();

    /**
     * @brief Set the maximum number of recent files to keep.
     */
    void setMaxRecentFiles(int max);

    /**
     * @brief Get the maximum number of recent files.
     */
    int maxRecentFiles() const;
    
    /**
     * @brief Load application translations.
     */
    void loadTranslations();
    
    /**
     * @brief Switch to a different language.
     */
    void switchLanguage(const QString &language);

private:
    QScopedPointer<ApplicationPrivate> d_ptr;
    Q_DECLARE_PRIVATE(Application)

    QStringList m_recentFiles;
    int m_maxRecentFiles = 10;
    QTranslator *m_translator = nullptr;
};

} // namespace QtWordEditor

#endif // APPLICATION_H