#include "errorhandler.h"

#include <iostream>

#if defined(_WIN32) || defined(_WIN64)
    #include <windows.h>
#elif defined(__APPLE__)
    #include <Security/Security.h>
#else
    #include <libsecret/secret.h>
#endif

// Logs a generic error message with context
void ErrorHandler::logError(const std::string& message, const std::string& context) {
    std::cerr << "Error: " << message;
    if (!context.empty()) {
        std::cerr << " | Context: " << context;
    }
    std::cerr << std::endl;
}

// Platform-specific error logging for Windows
void ErrorHandler::logWindowsError(const std::string& message) {
    DWORD error = GetLastError();
    std::cerr << "Windows Error: " << message << " | Error code: " << error << std::endl;
}

// Platform-specific error logging for macOS
void ErrorHandler::logMacOSSError(const std::string& message) {
    std::cerr << "macOS Error: " << message << std::endl;
}

// Platform-specific error logging for Linux
void ErrorHandler::logLinuxError(const std::string& message) {
    std::cerr << "Linux Error: " << message << std::endl;
}
