#include "core/utils/Logger.h"
#include "ui/widgets/DebugConsole.h"
#include <iostream>
#include <locale>
#include <codecvt>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

namespace QtWordEditor {

namespace {
    // 初始化控制台编码的静态初始化器
    struct ConsoleEncodingInitializer {
        ConsoleEncodingInitializer() {
#ifdef Q_OS_WIN
            // 设置 Windows 控制台编码为 UTF-8
            SetConsoleOutputCP(CP_UTF8);
            SetConsoleCP(CP_UTF8);
            
            // 设置 C++ 标准流的本地环境为 UTF-8
            try {
                // 使用 UTF-8 编码的 locale
                std::locale utf8_locale(std::locale(), new std::codecvt_utf8<wchar_t>);
                std::cout.imbue(utf8_locale);
                std::cerr.imbue(utf8_locale);
            } catch (...) {
                // 如果设置失败，忽略异常
            }
#endif
        }
    };
    
    // 静态初始化器，在程序启动时自动执行
    static ConsoleEncodingInitializer initializer;
}

bool Logger::s_debugEnabled = true;

void Logger::setDebugEnabled(bool enabled)
{
    s_debugEnabled = enabled;
}

bool Logger::isDebugEnabled()
{
    return s_debugEnabled;
}

namespace {
    // 检测是否是交互式控制台
    bool isInteractiveConsole()
    {
#ifdef Q_OS_WIN
        HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hStdOut == INVALID_HANDLE_VALUE) {
            return false;
        }
        
        DWORD mode;
        // 尝试获取控制台模式，如果成功说明是交互式控制台
        if (GetConsoleMode(hStdOut, &mode)) {
            return true;
        }
        
        // 检测输出是否被重定向
        DWORD fileType = GetFileType(hStdOut);
        return (fileType == FILE_TYPE_CHAR);
#else
        return isatty(fileno(stdout));
#endif
    }
    
    // 输出字符串到控制台（Windows 优先 WriteConsoleW，否则 std::cout）
    void outputToConsole(const QString& prefix, const QString& message)
    {
#ifdef Q_OS_WIN
        QString fullMessage = prefix + message + "\n";
        
        if (isInteractiveConsole()) {
            // 交互式控制台：使用 WriteConsoleW 直接输出 Unicode
            std::wstring wstr = fullMessage.toStdWString();
            HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
            if (hStdOut != INVALID_HANDLE_VALUE) {
                DWORD written;
                WriteConsoleW(hStdOut, wstr.c_str(), static_cast<DWORD>(wstr.length()), &written, nullptr);
            }
        } else {
            // 非交互式控制台（如 IDE 终端、重定向输出）：使用 std::cout 输出 UTF-8
            std::cout << fullMessage.toUtf8().constData();
            std::cout.flush();
        }
#else
        // 其他平台：使用 std::cout
        std::cout << prefix.toStdString() << message.toStdString() << std::endl;
#endif
    }
}

void Logger::debug(const QString& message)
{
#ifdef QT_DEBUG
    if (s_debugEnabled) {
        outputToConsole("[DEBUG] ", message);
        DebugConsole::debug(message);
    }
#endif
}

void Logger::debug(const char* message)
{
    debug(QString::fromUtf8(message));
}

void Logger::info(const QString& message)
{
    outputToConsole("[INFO] ", message);
    DebugConsole::info(message);
}

void Logger::warning(const QString& message)
{
    outputToConsole("[WARNING] ", message);
    DebugConsole::warning(message);
}

void Logger::error(const QString& message)
{
#ifdef Q_OS_WIN
    QString fullMessage = "[ERROR] " + message + "\n";
    
    if (isInteractiveConsole()) {
        // 交互式控制台：使用 WriteConsoleW 直接输出 Unicode 到 stderr
        std::wstring wstr = fullMessage.toStdWString();
        HANDLE hStdErr = GetStdHandle(STD_ERROR_HANDLE);
        if (hStdErr != INVALID_HANDLE_VALUE) {
            DWORD written;
            WriteConsoleW(hStdErr, wstr.c_str(), static_cast<DWORD>(wstr.length()), &written, nullptr);
        }
    } else {
        // 非交互式控制台（如 IDE 终端、重定向输出）：使用 std::cerr 输出 UTF-8
        std::cerr << fullMessage.toUtf8().constData();
        std::cerr.flush();
    }
#else
    std::cerr << "[ERROR] " << message.toStdString() << std::endl;
#endif
    
    DebugConsole::error(message);
}

} // namespace QtWordEditor
