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

class ApplicationPrivate
{
public:
    QSettings *settings = nullptr;
    QString organizationName = "QtWordEditor";
    QString applicationName = "QtWordEditor";
    QString version = "1.0.0";
};

Application::Application(int &argc, char **argv)
    : QApplication(argc, argv)
    , d_ptr(new ApplicationPrivate)
{
    setOrganizationName(d_ptr->organizationName);
    setApplicationName(d_ptr->applicationName);
    setApplicationVersion(d_ptr->version);
}

Application::~Application()
{
    saveSettings();
}

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

void Application::loadSettings()
{
    Q_D(Application);
    if (!d->settings) {
        d->settings = new QSettings(this);
    }
    // Load recent files, window geometry, etc.
    // For now, just a placeholder.
}

void Application::saveSettings()
{
    Q_D(Application);
    if (d->settings) {
        d->settings->sync();
    }
}

Application *Application::instance()
{
    return static_cast<Application*>(QCoreApplication::instance());
}

void Application::loadTranslations()
{
    // 移除旧的翻译器
    if (m_translator) {
        removeTranslator(m_translator);
        delete m_translator;
        m_translator = nullptr;
    }
    
    // 创建新的翻译器
    m_translator = new QTranslator(this);
    
    // 获取系统语言
    QString locale = QLocale::system().name();
    
    // 尝试从应用程序目录加载翻译
    QString translationsPath = QDir(applicationDirPath()).absoluteFilePath("translations");
    if (m_translator->load(QString("QtWordEditor_%1").arg(locale), translationsPath)) {
        installTranslator(m_translator);
        return;
    }
    
    // 如果失败，尝试从资源文件加载（如果有的话）
    if (m_translator->load(QString(":/translations/QtWordEditor_%1.qm").arg(locale))) {
        installTranslator(m_translator);
        return;
    }
    
    // 如果还是失败，删除翻译器
    delete m_translator;
    m_translator = nullptr;
}

void Application::switchLanguage(const QString &language)
{
    // 移除当前翻译器
    if (m_translator) {
        removeTranslator(m_translator);
        delete m_translator;
        m_translator = nullptr;
    }
    
    // 创建新的翻译器
    m_translator = new QTranslator(this);
    
    // 加载指定语言的翻译
    QString translationsPath = QDir(applicationDirPath()).absoluteFilePath("translations");
    if (m_translator->load(QString("QtWordEditor_%1").arg(language), translationsPath)) {
        installTranslator(m_translator);
    } else {
        // 如果加载失败，也删除翻译器
        delete m_translator;
        m_translator = nullptr;
    }
}

} // namespace QtWordEditor