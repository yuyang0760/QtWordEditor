
/**
 * @file Application.cpp
 * @brief Implementation of the Application class
 *
 * This file contains the implementation of the Application class which manages
 * the global application instance, settings, translations, and recent files.
 */

#include "app/Application.h"
#include &lt;QSettings&gt;
#include &lt;QMessageBox&gt;
#include &lt;QFileInfo&gt;
#include &lt;QDir&gt;
#include &lt;QTranslator&gt;
#include &lt;QLibraryInfo&gt;
#include &lt;QStandardPaths&gt;
#include &lt;QLocale&gt;

namespace QtWordEditor {

/**
 * @brief Private implementation class for Application
 */
class ApplicationPrivate
{
public:
    QSettings *settings = nullptr;
    QString organizationName = "QtWordEditor";
    QString applicationName = "QtWordEditor";
    QString version = "1.0.0";
};

/**
 * @brief Constructs the Application object
 * @param argc Number of command line arguments
 * @param argv Command line arguments
 */
Application::Application(int &amp;argc, char **argv)
    : QApplication(argc, argv)
    , d_ptr(new ApplicationPrivate)
{
    setOrganizationName(d_ptr-&gt;organizationName);
    setApplicationName(d_ptr-&gt;applicationName);
    setApplicationVersion(d_ptr-&gt;version);
}

/**
 * @brief Destroys the Application object and saves settings
 */
Application::~Application()
{
    saveSettings();
}

/**
 * @brief Initializes application components
 * @return true if initialization successful, false otherwise
 */
bool Application::init()
{
    try {
        loadSettings();
        loadTranslations();
        return true;
    } catch (const std::exception &amp;e) {
        QMessageBox::critical(nullptr, tr("Initialization Error"),
                              tr("Failed to initialize application: %1").arg(e.what()));
        return false;
    }
}

/**
 * @brief Loads application settings from persistent storage
 */
void Application::loadSettings()
{
    Q_D(Application);
    if (!d-&gt;settings) {
        d-&gt;settings = new QSettings(this);
    }
}

/**
 * @brief Saves application settings to persistent storage
 */
void Application::saveSettings()
{
    Q_D(Application);
    if (d-&gt;settings) {
        d-&gt;settings-&gt;sync();
    }
}

/**
 * @brief Returns the global application instance
 * @return Pointer to the Application instance
 */
Application *Application::instance()
{
    return static_cast&lt;Application*&gt;(QCoreApplication::instance());
}

/**
 * @brief Loads application translations based on system language
 */
void Application::loadTranslations()
{
    if (m_translator) {
        removeTranslator(m_translator);
        delete m_translator;
        m_translator = nullptr;
    }
    
    m_translator = new QTranslator(this);
    
    QString locale = QLocale::system().name();
    
    QStringList searchPaths;
    searchPaths &lt;&lt; QDir(applicationDirPath()).absoluteFilePath("translations");
    searchPaths &lt;&lt; QDir(applicationDirPath()).absolutePath();
    searchPaths &lt;&lt; QDir(qApp-&gt;applicationDirPath()).absoluteFilePath("cmake-build-debug");
    searchPaths &lt;&lt; QDir(qApp-&gt;applicationDirPath()).absoluteFilePath("../cmake-build-debug");
    
    for (const QString &amp;path : searchPaths) {
        qDebug() &lt;&lt; "Trying to load translation from:" &lt;&lt; path;
        if (m_translator-&gt;load(QString("QtWordEditor_%1").arg(locale), path)) {
            installTranslator(m_translator);
            qDebug() &lt;&lt; "Successfully loaded translation:" &lt;&lt; locale &lt;&lt; "from" &lt;&lt; path;
            return;
        }
        if (m_translator-&gt;load(QString("QtWordEditor_%1.qm").arg(locale), path)) {
            installTranslator(m_translator);
            qDebug() &lt;&lt; "Successfully loaded translation (with .qm):" &lt;&lt; locale &lt;&lt; "from" &lt;&lt; path;
            return;
        }
    }
    
    for (const QString &amp;path : searchPaths) {
        if (m_translator-&gt;load("QtWordEditor", path)) {
            installTranslator(m_translator);
            qDebug() &lt;&lt; "Successfully loaded translation without locale from" &lt;&lt; path;
            return;
        }
    }
    
    qDebug() &lt;&lt; "Failed to load translation";
    delete m_translator;
    m_translator = nullptr;
}

/**
 * @brief Switches the application language
 * @param language Language code (e.g., "zh_CN", "en_US")
 */
void Application::switchLanguage(const QString &amp;language)
{
    if (m_translator) {
        removeTranslator(m_translator);
        delete m_translator;
        m_translator = nullptr;
    }
    
    if (language == "en_US" || language == "en") {
        qDebug() &lt;&lt; "Switching to English";
        return;
    }
    
    m_translator = new QTranslator(this);
    
    QStringList searchPaths;
    searchPaths &lt;&lt; QDir(applicationDirPath()).absoluteFilePath("translations");
    searchPaths &lt;&lt; QDir(applicationDirPath()).absolutePath();
    searchPaths &lt;&lt; QDir(qApp-&gt;applicationDirPath()).absoluteFilePath("cmake-build-debug");
    searchPaths &lt;&lt; QDir(qApp-&gt;applicationDirPath()).absoluteFilePath("../cmake-build-debug");
    
    for (const QString &amp;path : searchPaths) {
        qDebug() &lt;&lt; "Trying to load translation from:" &lt;&lt; path;
        if (m_translator-&gt;load(QString("QtWordEditor_%1").arg(language), path)) {
            installTranslator(m_translator);
            qDebug() &lt;&lt; "Successfully loaded translation:" &lt;&lt; language &lt;&lt; "from" &lt;&lt; path;
            return;
        }
        if (m_translator-&gt;load(QString("QtWordEditor_%1.qm").arg(language), path)) {
            installTranslator(m_translator);
            qDebug() &lt;&lt; "Successfully loaded translation (with .qm):" &lt;&lt; language &lt;&lt; "from" &lt;&lt; path;
            return;
        }
    }
    
    qDebug() &lt;&lt; "Failed to load translation for" &lt;&lt; language;
    delete m_translator;
    m_translator = nullptr;
}

} // namespace QtWordEditor

