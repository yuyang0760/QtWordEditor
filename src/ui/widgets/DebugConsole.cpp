#include "ui/widgets/DebugConsole.h"
#include <QDateTime>
#include <QDebug>

namespace QtWordEditor {

DebugConsole *DebugConsole::s_instance = nullptr;
QtMessageHandler DebugConsole::s_oldMessageHandler = nullptr;

DebugConsole::DebugConsole(QObject *parent)
    : QObject(parent)
    , m_enabled(true)
    , m_processTimer(new QTimer(this))
{
    // 设置定时器，每 50 毫秒处理一次队列
    connect(m_processTimer, &QTimer::timeout, this, &DebugConsole::processQueuedMessages);
    m_processTimer->start(50);
}

DebugConsole *DebugConsole::instance()
{
    if (!s_instance) {
        s_instance = new DebugConsole();
    }
    return s_instance;
}

void DebugConsole::debugMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    Q_UNUSED(context);
    
    LogMessageType logType = LogMessageType::Debug;
    switch (type) {
        case QtDebugMsg:
            logType = LogMessageType::Debug;
            break;
        case QtInfoMsg:
            logType = LogMessageType::Info;
            break;
        case QtWarningMsg:
            logType = LogMessageType::Warning;
            break;
        case QtCriticalMsg:
            logType = LogMessageType::Error;
            break;
        case QtFatalMsg:
            logType = LogMessageType::Critical;
            break;
    }
    
    DebugConsole::instance()->enqueueMessage(msg, logType);
    
    // 同时输出到旧的消息处理器（如果有的话）
    if (s_oldMessageHandler) {
        s_oldMessageHandler(type, context, msg);
    }
}

void DebugConsole::installMessageHandler()
{
    if (!s_oldMessageHandler) {
        s_oldMessageHandler = qInstallMessageHandler(DebugConsole::debugMessageHandler);
    }
}

void DebugConsole::uninstallMessageHandler()
{
    if (s_oldMessageHandler) {
        qInstallMessageHandler(s_oldMessageHandler);
        s_oldMessageHandler = nullptr;
    }
}

QColor DebugConsole::colorForType(LogMessageType type)
{
    switch (type) {
        case LogMessageType::Debug:
            return QColor(0, 0, 0);          // 黑色，用户要求
        case LogMessageType::Info:
            return QColor(0, 150, 255);     // 亮蓝色，更鲜艳
        case LogMessageType::Warning:
            return QColor(255, 165, 0);     // 标准橙色，更鲜艳
        case LogMessageType::Error:
            return QColor(255, 0, 0);        // 纯红色，更鲜艳
        case LogMessageType::Critical:
            return QColor(255, 0, 127);      // 亮玫红色，更鲜艳
    }
    return QColor(0, 0, 0);
}

QString DebugConsole::typeName(LogMessageType type)
{
    switch (type) {
        case LogMessageType::Debug:
            return "DEBUG";
        case LogMessageType::Info:
            return "INFO";
        case LogMessageType::Warning:
            return "WARNING";
        case LogMessageType::Error:
            return "ERROR";
        case LogMessageType::Critical:
            return "CRITICAL";
    }
    return "";
}

void DebugConsole::enqueueMessage(const QString &text, LogMessageType type)
{
    QMutexLocker locker(&m_mutex);
    
    LogMessage msg;
    // 添加消息类型前缀，如 [DEBUG]、[INFO] 等
    QString prefix = "[" + typeName(type) + "] ";
    msg.text = prefix + text;
    msg.type = type;
    msg.color = colorForType(type);
    
    m_messageQueue.enqueue(msg);
}

void DebugConsole::processQueuedMessages()
{
    QMutexLocker locker(&m_mutex);
    
    if (m_messageQueue.isEmpty()) {
        return;
    }
    
    // 批量处理队列中的所有消息（最多 100 条，防止 UI 卡顿）
    int count = 0;
    while (!m_messageQueue.isEmpty() && count < 100) {
        LogMessage msg = m_messageQueue.dequeue();
        
        // 发送新的信号
        emit logMessage(msg);
        // 同时发送旧格式的信号以保持兼容
        emit logMessage(msg.text);
        
        count++;
    }
}

void DebugConsole::log(const QString &message)
{
    DebugConsole::instance()->enqueueMessage(message, LogMessageType::Debug);
}

void DebugConsole::debug(const QString &message)
{
    DebugConsole::instance()->enqueueMessage(message, LogMessageType::Debug);
}

void DebugConsole::info(const QString &message)
{
    DebugConsole::instance()->enqueueMessage(message, LogMessageType::Info);
}

void DebugConsole::warning(const QString &message)
{
    DebugConsole::instance()->enqueueMessage(message, LogMessageType::Warning);
}

void DebugConsole::error(const QString &message)
{
    DebugConsole::instance()->enqueueMessage(message, LogMessageType::Error);
}

void DebugConsole::critical(const QString &message)
{
    DebugConsole::instance()->enqueueMessage(message, LogMessageType::Critical);
}

void DebugConsole::clear()
{
    LogMessage clearMsg;
    clearMsg.text = "[控制台已清空]";
    clearMsg.type = LogMessageType::Info;
    clearMsg.color = colorForType(LogMessageType::Info);
    
    emit instance()->logMessage(clearMsg);
    emit instance()->logMessage(clearMsg.text);
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
