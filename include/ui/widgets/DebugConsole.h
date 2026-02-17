#ifndef DEBUGCONSOLE_H
#define DEBUGCONSOLE_H

#include <QObject>
#include <QString>

namespace QtWordEditor {

/**
 * @brief The DebugConsole class provides a centralized logging system for debugging.
 */
class DebugConsole : public QObject
{
    Q_OBJECT

public:
    static DebugConsole *instance();

    static void log(const QString &message);
    static void clear();
    static void setEnabled(bool enabled);
    static bool isEnabled();
    
    static void installMessageHandler();
    static void uninstallMessageHandler();

signals:
    void logMessage(const QString &message);

private:
    explicit DebugConsole(QObject *parent = nullptr);
    ~DebugConsole() override = default;

    bool m_enabled;
    static DebugConsole *s_instance;
    static QtMessageHandler s_oldMessageHandler;
};

} // namespace QtWordEditor

#endif // DEBUGCONSOLE_H
