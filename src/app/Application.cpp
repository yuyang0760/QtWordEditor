
/**
 * @file Application.cpp
 * @brief Implementation of the Application class
 *
 * This file contains the implementation of the Application class which manages
 * the global application instance, settings, translations, and recent files.
 */

#include "app/Application.h"
#include <QSettings>
#include <QMessageBox>
#include <QFileInfo>
#include <QDir>
#include <QTranslator>
#include <QLibraryInfo>
#include <QStandardPaths>
#include <QLocale>

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
Application::Application(int &argc, char **argv)
    : QApplication(argc, argv)
    , d_ptr(new ApplicationPrivate)
{
    setOrganizationName(d_ptr->organizationName);
    setApplicationName(d_ptr->applicationName);
    setApplicationVersion(d_ptr->version);
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
    } catch (const std::exception &e) {
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
    if (!d->settings) {
        d->settings = new QSettings(this);
    }
}

/**
 * @brief Saves application settings to persistent storage
 */
void Application::saveSettings()
{
    Q_D(Application);
    if (d->settings) {
        d->settings->sync();
    }
}

/**
 * @brief Returns the global application instance
 * @return Pointer to the Application instance
 */
Application *Application::instance()
{
    return static_cast<Application*>(QCoreApplication::instance());
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
    searchPaths << QDir(applicationDirPath()).absoluteFilePath("translations");
    searchPaths << QDir(applicationDirPath()).absolutePath();
    searchPaths << QDir(qApp->applicationDirPath()).absoluteFilePath("cmake-build-debug");
    searchPaths << QDir(qApp->applicationDirPath()).absoluteFilePath("../cmake-build-debug");
    
    for (const QString &path : searchPaths) {
        qDebug() << "Trying to load translation from:" << path;
        if (m_translator->load(QString("QtWordEditor_%1").arg(locale), path)) {
            installTranslator(m_translator);
            qDebug() << "Successfully loaded translation:" << locale << "from" << path;
            return;
        }
        if (m_translator->load(QString("QtWordEditor_%1.qm").arg(locale), path)) {
            installTranslator(m_translator);
            qDebug() << "Successfully loaded translation (with .qm):" << locale << "from" << path;
            return;
        }
    }
    
    for (const QString &path : searchPaths) {
        if (m_translator->load("QtWordEditor", path)) {
            installTranslator(m_translator);
            qDebug() << "Successfully loaded translation without locale from" << path;
            return;
        }
    }
    
    qDebug() << "Failed to load translation";
    delete m_translator;
    m_translator = nullptr;
}

/**
 * @brief Switches the application language
 * @param language Language code (e.g., "zh_CN", "en_US")
 */
void Application::switchLanguage(const QString &language)
{
    if (m_translator) {
        removeTranslator(m_translator);
        delete m_translator;
        m_translator = nullptr;
    }
    
    if (language == "en_US" || language == "en") {
        qDebug() << "Switching to English";
        return;
    }
    
    m_translator = new QTranslator(this);
    
    QStringList searchPaths;
    searchPaths << QDir(applicationDirPath()).absoluteFilePath("translations");
    searchPaths << QDir(applicationDirPath()).absolutePath();
    searchPaths << QDir(qApp->applicationDirPath()).absoluteFilePath("cmake-build-debug");
    searchPaths << QDir(qApp->applicationDirPath()).absoluteFilePath("../cmake-build-debug");
    
    for (const QString &path : searchPaths) {
        qDebug() << "Trying to load translation from:" << path;
        if (m_translator->load(QString("QtWordEditor_%1").arg(language), path)) {
            installTranslator(m_translator);
            qDebug() << "Successfully loaded translation:" << language << "from" << path;
            return;
        }
        if (m_translator->load(QString("QtWordEditor_%1.qm").arg(language), path)) {
            installTranslator(m_translator);
            qDebug() << "Successfully loaded translation (with .qm):" << language << "from" << path;
            return;
        }
    }
    
    qDebug() << "Failed to load translation for" << language;
    delete m_translator;
    m_translator = nullptr;
}

} // namespace QtWordEditor

