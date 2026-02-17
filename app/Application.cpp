#include "Application.h"
#include <QSettings>
#include <QMessageBox>
#include <QFileInfo>
#include <QDir>

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
        // Additional initialization (translations, plugins, etc.) can be added here.
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

} // namespace QtWordEditor