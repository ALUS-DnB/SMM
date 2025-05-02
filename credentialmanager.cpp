#include "credentialmanager.h"

#include <iostream>
#include <string>

#if defined(_WIN32) || defined(_WIN64)
    #include <windows.h>
    #include <wincred.h>
    #pragma comment(lib, "Advapi32.lib")
#elif defined(__APPLE__)
    #include <Security/Security.h>
#else
    #include <libsecret/secret.h>
    #include <glib.h>
#endif

class CredentialManager {
public:
    CredentialManager() {
        // Initialization (if needed for future expansion)
    }

    bool storeToken(const std::string& service, const std::string& username, const std::string& token) {
#if defined(_WIN32) || defined(_WIN64)
        std::wstring targetName = std::wstring(service.begin(), service.end()) + L":" + std::wstring(username.begin(), username.end());
        CREDENTIALW cred = { 0 };
        cred.Type = CRED_TYPE_GENERIC;
        cred.TargetName = const_cast<LPWSTR>(targetName.c_str());
        cred.CredentialBlobSize = static_cast<DWORD>(token.size());
        cred.CredentialBlob = (LPBYTE)token.data();
        cred.Persist = CRED_PERSIST_LOCAL_MACHINE;
        cred.UserName = const_cast<LPWSTR>(std::wstring(username.begin(), username.end()).c_str());

        if (!CredWriteW(&cred, 0)) {
            ErrorHandler::logWindowsError("Failed to store credential");
            return false;
        }
        return true;

#elif defined(__APPLE__)
        SecKeychainItemRef itemRef = nullptr;
        OSStatus status = SecKeychainAddGenericPassword(
            nullptr,
            static_cast<UInt32>(service.size()), service.c_str(),
            static_cast<UInt32>(username.size()), username.c_str(),
            static_cast<UInt32>(token.size()), token.c_str(),
            &itemRef
        );
        if (itemRef) CFRelease(itemRef);
        if (status != errSecSuccess) {
            ErrorHandler::logMacOSSError("Failed to store token in keychain");
            return false;
        }
        return true;

#else
        const SecretSchema schema = {
            "org.example.TokenStore", SECRET_SCHEMA_NONE,
            {
                { "service", SECRET_SCHEMA_ATTRIBUTE_STRING },
                { "username", SECRET_SCHEMA_ATTRIBUTE_STRING },
                { nullptr, static_cast<SecretSchemaAttributeType>(0) }
            }
        };

        GError* error = nullptr;
        secret_password_store_sync(&schema, SECRET_COLLECTION_DEFAULT, service.c_str(), token.c_str(),
                                   nullptr, &error,
                                   "service", service.c_str(),
                                   "username", username.c_str(),
                                   nullptr);

        if (error) {
            ErrorHandler::logLinuxError("Failed to store token with libsecret");
            g_error_free(error);
            return false;
        }
        return true;
#endif
    }

    std::string retrieveToken(const std::string& service, const std::string& username) {
#if defined(_WIN32) || defined(_WIN64)
        std::wstring targetName = std::wstring(service.begin(), service.end()) + L":" + std::wstring(username.begin(), username.end());
        PCREDENTIALW pcred;
        if (CredReadW(targetName.c_str(), CRED_TYPE_GENERIC, 0, &pcred)) {
            std::string token(reinterpret_cast<char*>(pcred->CredentialBlob), pcred->CredentialBlobSize);
            CredFree(pcred);
            return token;
        } else {
            ErrorHandler::logWindowsError("Failed to retrieve credential");
            return "";
        }

#elif defined(__APPLE__)
        UInt32 tokenLen;
        void* tokenData = nullptr;

        OSStatus status = SecKeychainFindGenericPassword(
            nullptr,
            static_cast<UInt32>(service.size()), service.c_str(),
            static_cast<UInt32>(username.size()), username.c_str(),
            &tokenLen, &tokenData,
            nullptr
        );

        if (status != errSecSuccess || tokenData == nullptr) {
            ErrorHandler::logMacOSSError("Failed to retrieve token from keychain");
            return "";
        }

        std::string token(reinterpret_cast<char*>(tokenData), tokenLen);
        SecKeychainItemFreeContent(nullptr, tokenData);
        return token;

#else
        const SecretSchema schema = {
            "org.example.TokenStore", SECRET_SCHEMA_NONE,
            {
                { "service", SECRET_SCHEMA_ATTRIBUTE_STRING },
                { "username", SECRET_SCHEMA_ATTRIBUTE_STRING },
                { nullptr, static_cast<SecretSchemaAttributeType>(0) }
            }
        };

        GError* error = nullptr;
        gchar* token = secret_password_lookup_sync(&schema, nullptr, &error,
                                                   "service", service.c_str(),
                                                   "username", username.c_str(),
                                                   nullptr);

        if (error) {
            ErrorHandler::logLinuxError("Failed to retrieve token from libsecret");
            g_error_free(error);
            return "";
        }

        std::string token_str = token ? token : "";
        g_free(token);
        return token_str;
#endif
    }

    bool deleteToken(const std::string& service, const std::string& username) {
#if defined(_WIN32) || defined(_WIN64)
        std::wstring targetName = std::wstring(service.begin(), service.end()) + L":" + std::wstring(username.begin(), username.end());
        if (!CredDeleteW(targetName.c_str(), CRED_TYPE_GENERIC, 0)) {
            ErrorHandler::logWindowsError("Failed to delete credential");
            return false;
        }
        return true;

#elif defined(__APPLE__)
        SecKeychainItemRef itemRef = nullptr;
        OSStatus status = SecKeychainFindGenericPassword(
            nullptr,
            static_cast<UInt32>(service.size()), service.c_str(),
            static_cast<UInt32>(username.size()), username.c_str(),
            nullptr, nullptr, &itemRef
        );

        if (status != errSecSuccess || itemRef == nullptr) {
            ErrorHandler::logMacOSSError("Failed to find item in keychain for deletion");
            return false;
        }

        status = SecKeychainItemDelete(itemRef);
        CFRelease(itemRef);

        if (status != errSecSuccess) {
            ErrorHandler::logMacOSSError("Failed to delete item from keychain");
            return false;
        }

        return true;

#else
        const SecretSchema schema = {
            "org.example.TokenStore", SECRET_SCHEMA_NONE,
            {
                { "service", SECRET_SCHEMA_ATTRIBUTE_STRING },
                { "username", SECRET_SCHEMA_ATTRIBUTE_STRING },
                { nullptr, static_cast<SecretSchemaAttributeType>(0) }
            }
        };

        GError* error = nullptr;
        gboolean success = secret_password_clear_sync(&schema, nullptr, &error,
                                                      "service", service.c_str(),
                                                      "username", username.c_str(),
                                                      nullptr);

        if (error) {
            ErrorHandler::logLinuxError("Failed to delete token with libsecret");
            g_error_free(error);
            return false;
        }

        return success;
#endif
    }
};
