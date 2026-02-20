#ifndef LOGGER_H
#define LOGGER_H

#include <QDebug>
#include <QString>

namespace QtWordEditor {

// 日志级别
enum class LogLevel {
    Debug,
    Info,
    Warning,
    Error
};

class Logger {
public:
    // 启用/禁用调试输出
    static void setDebugEnabled(bool enabled);
    static bool isDebugEnabled();
    
    // 调试输出（仅在调试模式下）
    static void debug(const QString& message);
    static void debug(const char* message);
    
    // 信息输出
    static void info(const QString& message);
    
    // 警告输出
    static void warning(const QString& message);
    
    // 错误输出
    static void error(const QString& message);
    
private:
    static bool s_debugEnabled;
};

} // namespace QtWordEditor

// 便捷宏
#ifdef QT_DEBUG
    #define LOG_DEBUG(msg) QtWordEditor::Logger::debug(msg)
#else
    #define LOG_DEBUG(msg) while(false) qDebug()
#endif

#define LOG_INFO(msg) QtWordEditor::Logger::info(msg)
#define LOG_WARNING(msg) QtWordEditor::Logger::warning(msg)
#define LOG_ERROR(msg) QtWordEditor::Logger::error(msg)

#endif // LOGGER_H
