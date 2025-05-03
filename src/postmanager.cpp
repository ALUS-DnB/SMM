// postmanager.cpp
#include "postmanager.h"
#include "poststorage.h"

#include <iostream>
#include <limits>
#include <filesystem>
#include <chrono>
#include <thread>
#include <sstream>

namespace fs = std::filesystem;

void PostManager::run() {
    loadPosts();
    while (true) {
        handleScheduledPosts();
        showMenu();

        int choice;
        std::cin >> choice;
        clearInputStream();

        switch (choice) {
            case 1: createPost(); break;
            case 2: viewPosts(); break;
            case 3: editPostByIndex(); break;
            case 4: postToSocialMedia(); break;
            case 5: schedulePostMenu(); break;
            case 6: savePosts(); break;
            case 7: setupToken(); break;
            case 8: std::cout << "👋 Goodbye!\n"; return;
            default: std::cout << "Invalid input. Try again.\n"; break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

void PostManager::showMenu() const {
    std::cout << "\n=== Social Media Post Manager ===\n"
              << "1. Create a new post\n"
              << "2. View all posts\n"
              << "3. Edit a post\n"
              << "4. Post to selected platforms\n"
              << "5. Schedule a post\n"
              << "6. Save posts to disk\n"
              << "7. Set up token\n"
              << "8. Exit\n"
              << "Choose an option: ";
}

void PostManager::createPost() {
    std::string message;
    do {
        std::cout << "Enter post message (max 280 chars): ";
        std::getline(std::cin, message);
        if (message.length() > 280) std::cout << "Message too long. Try again.\n";
    } while (message.length() > 280);

    std::string media;
    do {
        std::cout << "Enter media file path (or leave blank): ";
        std::getline(std::cin, media);
        if (!media.empty() && !fs::exists(media)) std::cout << "File doesn't exist. Try again.\n";
        else break;
    } while (true);

    Post newPost(message, media);

    std::cout << "Enter tags (space-separated): ";
    std::string tagLine;
    std::getline(std::cin, tagLine);
    std::istringstream ss(tagLine);
    std::string tag;
    while (ss >> tag) newPost.addTag(tag);

    posts.push_back(newPost);
    std::cout << "✅ Post created.\n";
}

void PostManager::viewPosts() const {
    if (posts.empty()) return (void)std::cout << "No posts.\n";

    for (size_t i = 0; i < posts.size(); ++i) {
        const auto& post = posts[i];
        std::cout << i+1 << ". " << post.message;
        if (!post.videoPath.empty()) std::cout << " [Media: " << post.videoPath << "]";
        if (post.scheduledTime) std::cout << " [Scheduled: " << std::ctime(&post.scheduledTime) << "]";
        std::cout << " Tags: ";
        for (const auto& tag : post.getTags()) std::cout << "#" << tag << " ";
        std::cout << "\n";
    }
}

void PostManager::editPostByIndex() {
    if (posts.empty()) return (void)std::cout << "No posts to edit.\n";
    std::cout << "Post number to edit: ";
    int index;
    std::cin >> index;
    clearInputStream();
    if (index < 1 || static_cast<size_t>(index) > posts.size()) return (void)std::cout << "Invalid index.\n";

    Post& post = posts[index - 1];
    std::string newMsg;
    std::cout << "New message (blank = keep): ";
    std::getline(std::cin, newMsg);
    if (!newMsg.empty() && newMsg.length() <= 280) post.message = newMsg;

    std::string newMedia;
    std::cout << "New media path (blank = keep): ";
    std::getline(std::cin, newMedia);
    if (!newMedia.empty() && fs::exists(newMedia)) post.videoPath = newMedia;
    std::cout << "✅ Post updated.\n";
}

void PostManager::schedulePostMenu() {
    if (posts.empty()) return (void)std::cout << "No posts to schedule.\n";
    int index, delay;
    std::cout << "Post number to schedule: "; std::cin >> index;
    clearInputStream();
    if (index < 1 || static_cast<size_t>(index) > posts.size()) return;
    std::cout << "Delay (seconds): "; std::cin >> delay;
    clearInputStream();
    if (delay <= 0) return;
    schedulePost(posts[index - 1], delay);
}

void PostManager::schedulePost(Post& post, int delayInSeconds) {
    post.scheduledTime = std::time(nullptr) + delayInSeconds;
    std::cout << "Post scheduled.\n";
}

void PostManager::handleScheduledPosts() {
    std::time_t now = std::time(nullptr);
    for (auto& post : posts) {
        if (post.scheduledTime && post.scheduledTime <= now) {
            std::cout << "Posting scheduled post...\n";
            auto platforms = getPlatformSelectionFromUser();
            for (const auto& platform : platforms) postToPlatform(post, platform);
            if (confirm("Delete this post after publishing? (y/n): ")) post = posts.back(), posts.pop_back();
            else post.scheduledTime = 0;
        }
    }
}

void PostManager::postToPlatform(const Post& post, const std::string& platform) const {
    auto tokenOpt = credentialManager.getToken(platform);
    if (!tokenOpt) return ErrorHandler::logError("Missing token for " + platform, "postToPlatform");
    SocialMediaCurl curl(*tokenOpt);
    bool success = false;
    if (platform == "twitter") success = curl.postToTwitter(post.message, post.videoPath);
    else if (platform == "facebook") success = curl.postToFacebook(post.message, post.videoPath);
    else if (platform == "instagram") success = curl.postToInstagram(post.message, post.videoPath);
    else if (platform == "tiktok") success = curl.postToTikTok(post.message, post.videoPath);
    else if (platform == "youtube") success = curl.postToYouTube(post.message, post.videoPath);

    std::cout << (success ? "✅ Posted to " + platform : "❌ Failed to post to " + platform) << "\n";
}

void PostManager::postToSocialMedia() {
    if (posts.empty()) return (void)std::cout << "No posts.\n";
    auto platforms = getPlatformSelectionFromUser();
    for (const auto& post : posts)
        for (const auto& platform : platforms)
            postToPlatform(post, platform);
}

void PostManager::savePosts() const {
    PostStorage::save(posts);
}

void PostManager::loadPosts() {
    posts = PostStorage::load();
}

std::vector<std::string> PostManager::getPlatformSelectionFromUser() const {
    std::vector<std::string> selected;
    std::cout << "Platforms: [1] Twitter [2] Facebook [3] Instagram [4] TikTok [5] YouTube\n";
    std::cout << "Enter numbers separated by space (e.g., 1 3 5): ";
    std::string input;
    std::getline(std::cin, input);
    std::istringstream ss(input);
    int val;
    while (ss >> val) {
        switch (val) {
            case 1: selected.push_back("twitter"); break;
            case 2: selected.push_back("facebook"); break;
            case 3: selected.push_back("instagram"); break;
            case 4: selected.push_back("tiktok"); break;
            case 5: selected.push_back("youtube"); break;
        }
    }
    return selected;
}

bool PostManager::confirm(const std::string& prompt) const {
    std::string response;
    std::cout << prompt;
    std::getline(std::cin, response);
    return response == "y" || response == "Y";
}

void PostManager::clearInputStream() const {
    if (!std::cin.eof()) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

void PostManager::setupToken() {
    std::string platform, token;
    std::cout << "Enter platform: "; std::getline(std::cin, platform);
    std::cout << "Enter token: "; std::getline(std::cin, token);
    if (credentialManager.storeToken(platform, token)) std::cout << "Token saved.\n";
    else std::cout << "Failed to save token.\n";
}
