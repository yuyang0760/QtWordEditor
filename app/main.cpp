#include "Application.h"
#include <QMessageBox>
#include <iostream>

int main(int argc, char *argv[])
{
    QtWordEditor::Application app(argc, argv);

    // Global exception handling
    try {
        if (!app.init()) {
            return 1;
        }

        // For now, just show a message that the app is starting.
        // Later, the main window will be created here.
        QMessageBox::information(nullptr, "QtWordEditor",
                                 "Application started successfully.\n"
                                 "Implementation of UI is pending.");

        return app.exec();
    } catch (const std::exception &e) {
        QMessageBox::critical(nullptr, "Fatal Error",
                              QString("Unhandled exception: %1").arg(e.what()));
        return 1;
    }
}