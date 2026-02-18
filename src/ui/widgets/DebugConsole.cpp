#include "ui/widgets/DebugConsole.h"
#include <QDateTime>
#include <QDebug>

namespace QtWordEditor {

DebugConsole *DebugConsole::s_instance = nullptr;
QtMessageHandler DebugConsole::s_oldMessageHandler = nullptr;

DebugConsole::DebugConsole(QObject *parent)
    : QObject(parent)
    , m_enabled(true)
{
}

DebugConsole *DebugConsole::instance()
{
    if (!s_instance) {
        s_instance = new DebugConsole();
    }
    return s_instance;
}

static void debugMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    Q_UNUSED(type);
    Q_UNUSED(context);
    
    DebugConsole::log(msg);
}

void DebugConsole::installMessageHandler()
{
    if (!s_oldMessageHandler) {
        s_oldMessageHandler = qInstallMessageHandler(debugMessageHandler);
    }
}

void DebugConsole::uninstallMessageHandler()
{
    if (s_oldMessageHandler) {
        qInstallMessageHandler(s_oldMessageHandler);
        s_oldMessageHandler = nullptr;
    }
}

void DebugConsole::log(const QString &message)
{
    DebugConsole *console = instance();
    if (!console->m_enabled)
        return;

    QString fullMessage = message;
    
    // 同时输出到旧的消息处理器（如果有的话）
    if (s_oldMessageHandler) {
        s_oldMessageHandler(QtDebugMsg, QMessageLogContext(), fullMessage);
    } else {
        qDebug().noquote() << fullMessage;
    }
    
    emit console->logMessage(fullMessage);
}

void DebugConsole::clear()
{
    emit instance()->logMessage("[控制台已清空]");
}

void DebugConsole::setEnabled(bool enabled)
{
    instance()->m_enabled = enabled;
}

bool DebugConsole::isEnabled()
{
    return instance()->m_enabled;
}

} // namespace QtWordEditor
