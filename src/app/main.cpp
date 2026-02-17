#include "app/Application.h"
#include "ui/mainwindow/MainWindow.h"
#include <QTranslator>
#include <QLibraryInfo>
#include <QDir>
#include <QStandardPaths>
#include <QStyleFactory>
#include <QDebug>
#include <iostream>

int main(int argc, char *argv[])
{
    QtWordEditor::Application app(argc, argv);
    
    qDebug() << "Available styles:" << QStyleFactory::keys();
    
    // 启用 Fusion 样式
    QStyle *fusionStyle = QStyleFactory::create("Fusion");
    if (fusionStyle) {
        app.setStyle(fusionStyle);
        qDebug() << "Fusion style applied successfully!";
    } else {
        qDebug() << "Failed to create Fusion style!";
    }
    
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

        // 创建并显示主窗口
        QtWordEditor::MainWindow mainWindow;
        mainWindow.show();

        return app.exec();
    } catch (const std::exception &e) {
        std::cerr << "Unhandled exception: " << e.what() << std::endl;
        return 1;
    }
}
