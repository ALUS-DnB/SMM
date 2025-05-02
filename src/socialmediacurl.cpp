#include "socialmediacurl.h"

#include <curl/curl.h>
#include <iostream>
#include <fstream>

SocialMediaCurl::SocialMediaCurl(CredentialManager& credentialManager)
    : credentialManager(credentialManager) {}

SocialMediaCurl::~SocialMediaCurl() {}

std::string SocialMediaCurl::getAuthHeader(const std::string& platform) {
    auto tokenOpt = credentialManager.getToken(platform);
    if (!tokenOpt.has_value()) {
        ErrorHandler::logError("Missing auth token", platform);  // Use existing error handler
        return "";
    }
    return "Authorization: Bearer " + tokenOpt.value();
}

bool SocialMediaCurl::sendPostRequest(const std::string& url, const std::string& postData, const std::string& platform) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        ErrorHandler::logError("Failed to initialize cURL", platform);  // Log initialization error
        return false;
    }

    std::string authHeader = getAuthHeader(platform);
    if (authHeader.empty()) return false;

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, authHeader.c_str());
    headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    CURLcode res = curl_easy_perform(curl);
    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

    if (res != CURLE_OK) {
        ErrorHandler::logError(curl_easy_strerror(res), platform);  // Log cURL error
    } else if (http_code >= 400) {
        ErrorHandler::logError("HTTP error " + std::to_string(http_code), platform);  // Log HTTP error
    }

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    return (res == CURLE_OK && http_code < 400);
}

bool SocialMediaCurl::sendMultipartRequest(const std::string& url, const std::string& message, const std::string& mediaPath, const std::string& platform) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        ErrorHandler::logError("Failed to initialize cURL for multipart request", platform);  // Log multipart cURL error
        return false;
    }

    std::string authHeader = getAuthHeader(platform);
    if (authHeader.empty()) return false;

    struct curl_httppost* form = nullptr;
    struct curl_httppost* last = nullptr;

    curl_formadd(&form, &last,
                 CURLFORM_COPYNAME, "message",
                 CURLFORM_COPYCONTENTS, message.c_str(),
                 CURLFORM_END);

    if (!mediaPath.empty()) {
        curl_formadd(&form, &last,
                     CURLFORM_COPYNAME, "media",
                     CURLFORM_FILE, mediaPath.c_str(),
                     CURLFORM_END);
    }

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, authHeader.c_str());

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPPOST, form);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    CURLcode res = curl_easy_perform(curl);
    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

    if (res != CURLE_OK) {
        ErrorHandler::logError(curl_easy_strerror(res), platform);  // Log cURL error
    } else if (http_code >= 400) {
        ErrorHandler::logError("HTTP error " + std::to_string(http_code), platform);  // Log HTTP error
    }

    curl_formfree(form);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    return (res == CURLE_OK && http_code < 400);
}

// Platform-specific implementations

bool SocialMediaCurl::postToTwitter(const std::string& message, const std::string& mediaPath) {
    std::string url = "https://api.twitter.com/2/tweets";
    std::string postData = "text=" + message;
    return sendPostRequest(url, postData, "twitter");
}

bool SocialMediaCurl::postToFacebook(const std::string& message, const std::string& mediaPath) {
    std::string url = "https://graph.facebook.com/v18.0/me/feed";
    std::string postData = "message=" + message;
    return sendPostRequest(url, postData, "facebook");
}

bool SocialMediaCurl::postToInstagram(const std::string& message, const std::string& mediaPath) {
    std::string url = "https://graph.facebook.com/v18.0/me/media";  // Requires long-lived Instagram token
    return sendMultipartRequest(url, message, mediaPath, "instagram");
}

bool SocialMediaCurl::postToTikTok(const std::string& message, const std::string& mediaPath) {
    std::string url = "https://open.tiktokapis.com/v2/post/publish/";  // Requires scoped access token
    return sendMultipartRequest(url, message, mediaPath, "tiktok");
}

bool SocialMediaCurl::postToYouTube(const std::string& message, const std::string& mediaPath) {
    std::string url = "https://www.googleapis.com/upload/youtube/v3/videos?uploadType=multipart&part=snippet,status";
    return sendMultipartRequest(url, message, mediaPath, "youtube");
}
