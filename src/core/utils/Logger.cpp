#include "core/utils/Logger.h"
#include <iostream>

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
        std::cout << "[DEBUG] " << message.toStdString() << std::endl;
    }
#endif
}

void Logger::debug(const char* message)
{
    debug(QString::fromUtf8(message));
}

void Logger::info(const QString& message)
{
    std::cout << "[INFO] " << message.toStdString() << std::endl;
}

void Logger::warning(const QString& message)
{
    std::cout << "[WARNING] " << message.toStdString() << std::endl;
}

void Logger::error(const QString& message)
{
    std::cerr << "[ERROR] " << message.toStdString() << std::endl;
}

} // namespace QtWordEditor
