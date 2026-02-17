#include "app/Application.h"
#include <QMessageBox>
#include <QTranslator>
#include <QLibraryInfo>
#include <QDir>
#include <QStandardPaths>
#include <iostream>

int main(int argc, char *argv[])
{
    QtWordEditor::Application app(argc, argv);
    
    // 加载翻译文件
    QTranslator translator;
    QTranslator qtTranslator;
    
    // 获取系统语言
    QString locale = QLocale::system().name();
    
    // 加载Qt内置翻译
    if (qtTranslator.load("qt_" + locale, QLibraryInfo::path(QLibraryInfo::TranslationsPath))) {
        app.installTranslator(&qtTranslator);
    }
    
    // 加载应用程序翻译
    QString translationsPath = QDir(QCoreApplication::applicationDirPath()).absoluteFilePath("translations");
    if (translator.load("QtWordEditor_" + locale, translationsPath)) {
        app.installTranslator(&translator);
    }
    
    // 全局异常处理
    try {
        if (!app.init()) {
            return 1;
        }

        // 目前只显示应用启动消息
        // 后续将在这里创建主窗口
        QMessageBox::information(nullptr, 
                                QObject::tr("QtWordEditor"),
                                QObject::tr("Application started successfully.\n"
                                          "Implementation of UI is pending."));

        return app.exec();
    } catch (const std::exception &e) {
        QMessageBox::critical(nullptr, 
                             QObject::tr("Fatal Error"),
                             QObject::tr("Unhandled exception: %1").arg(e.what()));
        return 1;
    }
}