
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
#include &lt;QTranslator&gt;
#include &lt;QLibraryInfo&gt;
#include &lt;QDir&gt;
#include &lt;QStandardPaths&gt;
#include &lt;QStyleFactory&gt;
#include &lt;QDebug&gt;
#include &lt;iostream&gt;

/**
 * @brief Main entry point of the application
 * @param argc Number of command line arguments
 * @param argv Command line arguments
 * @return Exit code (0 for success, 1 for failure)
 */
int main(int argc, char *argv[])
{
    QtWordEditor::Application app(argc, argv);
    
    qDebug() &lt;&lt; "Available styles:" &lt;&lt; QStyleFactory::keys();
    
    // Enable Fusion style for consistent look across platforms
    QStyle *fusionStyle = QStyleFactory::create("Fusion");
    if (fusionStyle) {
        app.setStyle(fusionStyle);
        qDebug() &lt;&lt; "Fusion style applied successfully!";
    } else {
        qDebug() &lt;&lt; "Failed to create Fusion style!";
    }
    
    // Load translation files
    QTranslator translator;
    QTranslator qtTranslator;
    
    // Get system language
    QString locale = QLocale::system().name();
    
    // Load Qt built-in translations
    if (qtTranslator.load("qt_" + locale, QLibraryInfo::path(QLibraryInfo::TranslationsPath))) {
        app.installTranslator(&amp;qtTranslator);
    }
    
    // Load application translations
    QString translationsPath = QDir(QCoreApplication::applicationDirPath()).absoluteFilePath("translations");
    if (translator.load("QtWordEditor_" + locale, translationsPath)) {
        app.installTranslator(&amp;translator);
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
    } catch (const std::exception &amp;e) {
        std::cerr &lt;&lt; "Unhandled exception: " &lt;&lt; e.what() &lt;&lt; std::endl;
        return 1;
    }
}

