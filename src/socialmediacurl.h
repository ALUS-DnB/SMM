#ifndef SOCIALMEDIACURL_H
#define SOCIALMEDIACURL_H

#include "credentialmanager.h"
#include "errorhandler.h"

#include <string>

class SocialMediaCurl {
public:
    explicit SocialMediaCurl(CredentialManager& credentialManager);
    ~SocialMediaCurl();

    bool postToTwitter(const std::string& message, const std::string& mediaPath);
    bool postToFacebook(const std::string& message, const std::string& mediaPath);
    bool postToInstagram(const std::string& message, const std::string& mediaPath);
    bool postToTikTok(const std::string& message, const std::string& mediaPath);
    bool postToYouTube(const std::string& message, const std::string& mediaPath);

private:
    CredentialManager& credentialManager;

    bool sendPostRequest(const std::string& url, const std::string& postData, const std::string& platform);
    bool sendMultipartRequest(const std::string& url, const std::string& message, const std::string& mediaPath, const std::string& platform);
    std::string getAuthHeader(const std::string& platform);  // Keep this private as it's only for internal use
};

#endif // SOCIALMEDIACURL_H
