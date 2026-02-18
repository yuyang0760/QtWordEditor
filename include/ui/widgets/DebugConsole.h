#ifndef DEBUGCONSOLE_H
#define DEBUGCONSOLE_H

#include <QObject>
#include <QString>
#include <QColor>
#include <QMutex>
#include <QQueue>
#include <QTimer>

namespace QtWordEditor {

/**
 * @brief 消息类型枚举
 */
enum class LogMessageType {
    Debug,      // 调试信息
    Info,       // 普通信息
    Warning,    // 警告
    Error,      // 错误
    Critical    // 严重错误
};

/**
 * @brief 日志消息结构体
 */
struct LogMessage {
    QString text;
    LogMessageType type;
    QColor color;
};

/**
 * @brief The DebugConsole class provides a centralized logging system for debugging.
 */
class DebugConsole : public QObject
{
    Q_OBJECT

public:
    static DebugConsole *instance();

    // 不同级别的日志方法
    static void log(const QString &message);
    static void debug(const QString &message);
    static void info(const QString &message);
    static void warning(const QString &message);
    static void error(const QString &message);
    static void critical(const QString &message);
    
    static void clear();
    static void setEnabled(bool enabled);
    static bool isEnabled();
    
    static void installMessageHandler();
    static void uninstallMessageHandler();

signals:
    // 新的信号：发送完整的 LogMessage
    void logMessage(const LogMessage &message);
    // 保留旧的信号以保持兼容
    void logMessage(const QString &message);

private slots:
    // 批量处理队列中的消息
    void processQueuedMessages();

private:
    explicit DebugConsole(QObject *parent = nullptr);
    ~DebugConsole() override = default;

    // 添加消息到队列
    void enqueueMessage(const QString &text, LogMessageType type);
    
    // 消息处理函数（静态成员函数）
    static void debugMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);

    bool m_enabled;
    QMutex m_mutex;
    QQueue<LogMessage> m_messageQueue;
    QTimer *m_processTimer;
    
    static DebugConsole *s_instance;
    static QtMessageHandler s_oldMessageHandler;
    
    // 获取消息类型对应的颜色
    static QColor colorForType(LogMessageType type);
    // 获取消息类型的名称
    static QString typeName(LogMessageType type);
};

} // namespace QtWordEditor

#endif // DEBUGCONSOLE_H
