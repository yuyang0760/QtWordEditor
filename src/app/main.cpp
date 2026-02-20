
/**
 * @file main.cpp
 * @brief Application entry point for QtWordEditor
 *
 * This file contains the main() function which initializes the application,
 * sets up translations, applies styles, creates the main window, and starts
 * the event loop.
 */

#include "app/Application.h"
#include "ui/mainwindow/MainWindow.h"
#include <QTranslator>
#include <QLibraryInfo>
#include <QDir>
#include <QStandardPaths>
#include <QStyleFactory>
#include <QDebug>
#include <iostream>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

/**
 * @brief Main entry point of the application
 * @param argc Number of command line arguments
 * @param argv Command line arguments
 * @return Exit code (0 for success, 1 for failure)
 */
int main(int argc, char *argv[])
{
#ifdef Q_OS_WIN
    // 设置 Windows 控制台编码为 UTF-8，解决中文乱码问题
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    QtWordEditor::Application app(argc, argv);
    
  //  QDebug() << "Available styles:" << QStyleFactory::keys();
    
    // Enable Fusion style for consistent look across platforms
    QStyle *fusionStyle = QStyleFactory::create("Fusion");
    if (fusionStyle) {
        app.setStyle(fusionStyle);
      //  QDebug() << "Fusion style applied successfully!";
    } else {
      //  QDebug() << "Failed to create Fusion style!";
    }
    
    // Load translation files
    QTranslator translator;
    QTranslator qtTranslator;
    
    // Get system language
    QString locale = QLocale::system().name();
    
    // Load Qt built-in translations
    if (qtTranslator.load("qt_" + locale, QLibraryInfo::path(QLibraryInfo::TranslationsPath))) {
        app.installTranslator(&qtTranslator);
    }
    
    // Load application translations
    QString translationsPath = QDir(QCoreApplication::applicationDirPath()).absoluteFilePath("translations");
    if (translator.load("QtWordEditor_" + locale, translationsPath)) {
        app.installTranslator(&translator);
    }
    
    // Global exception handling
    try {
        if (!app.init()) {
            return 1;
        }

        // Create and show main window
        QtWordEditor::MainWindow mainWindow;
        mainWindow.show();

        return app.exec();
    } catch (const std::exception &e) {
        std::cerr << "Unhandled exception: " << e.what() << std::endl;
        return 1;
    }
}

