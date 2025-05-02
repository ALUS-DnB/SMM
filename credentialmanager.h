#ifndef CREDENTIAL_MANAGER_H
#define CREDENTIAL_MANAGER_H

#include "errorhandler.h"

#include <string>
#include <optional>

#if defined(_WIN32) || defined(_WIN64)
// Windows-specific declarations (if necessary)
#elif defined(__APPLE__)
// macOS-specific declarations (if necessary)
#else
// Linux-specific declarations using libsecret
#endif

// Main CredentialManager class to handle platform-specific storage/retrieval
class CredentialManager {
public:
    // Stores a token for a given platform
    static bool storeToken(const std::string& service, const std::string& username, const std::string& token);

    // Retrieves a token for a given platform
    static std::optional<std::string> getToken(const std::string& service, const std::string& username);

    // Deletes a token for a given platform
    static bool deleteToken(const std::string& service, const std::string& username);
};

#endif // CREDENTIAL_MANAGER_H
