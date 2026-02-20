#include "core/utils/Logger.h"

namespace QtWordEditor {

bool Logger::s_debugEnabled = true;

void Logger::setDebugEnabled(bool enabled)
{
    s_debugEnabled = enabled;
}

bool Logger::isDebugEnabled()
{
    return s_debugEnabled;
}

void Logger::debug(const QString& message)
{
#ifdef QT_DEBUG
    if (s_debugEnabled) {
        qDebug() << "[DEBUG]" << message;
    }
#endif
}

void Logger::debug(const char* message)
{
    debug(QString::fromLatin1(message));
}

void Logger::info(const QString& message)
{
    qInfo() << "[INFO]" << message;
}

void Logger::warning(const QString& message)
{
    qWarning() << "[WARNING]" << message;
}

void Logger::error(const QString& message)
{
    qCritical() << "[ERROR]" << message;
}

} // namespace QtWordEditor
