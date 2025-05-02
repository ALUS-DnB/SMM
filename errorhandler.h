#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

#include <string>

class ErrorHandler {
public:
    // Log error message with context
    static void logError(const std::string& message, const std::string& context = "");

    // Log platform-specific errors
    static void logWindowsError(const std::string& message);
    static void logMacOSSError(const std::string& message);
    static void logLinuxError(const std::string& message);
};

#endif // ERROR_HANDLER_H