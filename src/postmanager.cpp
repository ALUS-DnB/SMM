#include "postmanager.h"

#include <iostream>
#include <limits>
#include <filesystem>
#include <chrono>
#include <thread>
#include <sstream>

namespace fs = std::filesystem;

void PostManager::run() {
    while (true) {
        handleScheduledPosts();
        showMenu();

        int choice = getValidatedChoice(1, 8);  // Updated max option to 8
        if (choice == -1) {
            std::cout << "⚠️  Invalid input. Please try again.\n";
            continue;  // Instead of breaking, let the user try again
        }

        switch (choice) {
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
                handleChoice(choice);
                break;
            case 7:
                std::cout << "👋 Goodbye!\n";
                return;
            case 8:
                setupToken();
                break;
        }

        std::cout << "\n🔁 Returning to main menu...\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

void PostManager::showMenu() const {
    std::cout << "\n=== Social Media Post Manager ===\n"
              << "1. Create a new post\n"
              << "2. View all posts\n"
              << "3. View post by number\n"
              << "4. Edit a post\n"
              << "5. Post to social media platforms\n"
              << "6. Schedule a post\n"
              << "7. Exit\n"
              << "8. Set up token\n"
              << "Choose an option: ";
}

void PostManager::createPost() {
    std::cout << "\nCreating a new post...\n";

    std::string message;
    std::cout << "Enter the message (max " << MAX_CHAR_LIMIT << " characters): ";
    std::getline(std::cin, message);

    if (message.length() > MAX_CHAR_LIMIT) {
        ErrorHandler::logError("Message too long. Post creation failed.", "createPost");
        return;
    }

    std::string videoPath;
    std::cout << "Enter media file path (or leave empty for no media): ";
    std::getline(std::cin, videoPath);

    if (!videoPath.empty() && !fs::exists(videoPath)) {
        ErrorHandler::logError("File does not exist. Post creation failed.", "createPost");
        return;
    }

    // Create a new post with message and media
    Post newPost(message, videoPath);

    // Ask for tags and add them to the post
    std::string tag;
    std::cout << "Enter tags (separate tags by spaces, or press Enter to finish): ";
    std::getline(std::cin, tag);

    if (!tag.empty()) {
        std::istringstream tagStream(tag);
        std::string individualTag;
        while (tagStream >> individualTag) {
            newPost.addTag(individualTag);  // Add each tag to the post
        }
    }

    posts.push_back(newPost);
    std::cout << "✅ New post created successfully!\n";
}

void PostManager::viewPosts() const {
    if (posts.empty()) {
        std::cout << "No posts to display.\n";
        return;
    }

    std::cout << "\n📋 Your Posts:\n";
    for (std::size_t i = 0; i < posts.size(); ++i) {
        const auto& post = posts[i];
        std::cout << i + 1 << ". " << post.message;
        if (!post.videoPath.empty()) {
            std::cout << " [🎥 Attached: " << post.videoPath << "]";
        }
        if (post.scheduledTime > 0) {
            std::cout << " [⏰ Scheduled: " << std::ctime(&post.scheduledTime) << "]";
        }

        // Display tags
        std::cout << " Tags: ";
        auto tags = post.getTags();
        if (!tags.empty()) {
            for (const auto& tag : tags) {
                std::cout << "#" << tag << " ";
            }
        } else {
            std::cout << "No tags.\n";
        }

        std::cout << "\n";
    }
}

void PostManager::editPostByIndex() {
    if (posts.empty()) {
        ErrorHandler::logError("No posts available to edit.", "editPostByIndex");
        return;
    }

    std::cout << "Enter post number to edit (1 to " << posts.size() << "): ";
    std::size_t index;
    std::cin >> index;

    if (std::cin.fail() || index < 1 || index > posts.size()) {
        ErrorHandler::logError("Invalid post number.", "editPostByIndex");
        clearInputStream();
        return;
    }

    clearInputStream(); 
    auto& post = posts[index - 1]; 

    std::cout << "Editing Post #" << index << ":\n";
    std::cout << "Current message: " << post.message << "\n";
    std::cout << "Enter new message (max " << MAX_CHAR_LIMIT << " characters, or leave empty to keep): ";
    std::string newMessage;
    std::getline(std::cin, newMessage);

    if (!newMessage.empty() && newMessage.length() <= MAX_CHAR_LIMIT) {
        post.message = std::move(newMessage);
    } else if (!newMessage.empty()) {
        ErrorHandler::logError("Message too long. Edit failed.", "editPostByIndex");
        return;
    }

    std::cout << "Current media file path: " << (post.videoPath.empty() ? "None" : post.videoPath) << "\n";
    std::cout << "Enter new media file path (or leave empty to keep): ";
    std::string newVideoPath;
    std::getline(std::cin, newVideoPath);

    if (!newVideoPath.empty() && !fs::exists(newVideoPath)) {
        ErrorHandler::logError("File does not exist. Edit failed.", "editPostByIndex");
        return;
    }

    if (!newVideoPath.empty()) {
        post.videoPath = std::move(newVideoPath);
    }

    std::cout << "✅ Post edited successfully.\n";
}

void PostManager::schedulePostMenu() {
    if (posts.empty()) {
        ErrorHandler::logError("No posts to schedule.", "schedulePostMenu");
        return;
    }

    std::size_t index;
    int delay;
    while (true) {
        std::cout << "Enter post number to schedule (1 to " << posts.size() << "): ";
        std::cin >> index;

        if (std::cin.fail() || index < 1 || index > posts.size()) {
            ErrorHandler::logError("Invalid post number.", "schedulePostMenu");
            clearInputStream();  // Clear the stream and prompt again
            continue;
        }

        clearInputStream();

        std::cout << "Enter delay in seconds: ";
        std::cin >> delay;

        if (std::cin.fail() || delay <= 0) {
            ErrorHandler::logError("Invalid delay value.", "schedulePostMenu");
            clearInputStream();  // Clear the stream and prompt again
            continue;
        }

        schedulePost(posts[index - 1], delay);
        break;  // Exit loop when valid input is received
    }
}

void PostManager::logAction(const std::string& actionMessage) const {
    std::cout << "Action Log: " << actionMessage << "\n";
}

void PostManager::logError(const std::string& errorMessage) const {
    std::cout << "Error Log: " << errorMessage << "\n";
}

void PostManager::schedulePost(Post& post, int delayInSeconds) {
    post.scheduledTime = std::time(nullptr) + delayInSeconds;
    std::cout << "Post scheduled for: " << std::ctime(&post.scheduledTime) << "\n";
}

void PostManager::handleScheduledPosts() {
    std::time_t currentTime = std::time(nullptr);
    for (auto& post : posts) {
        if (post.scheduledTime != 0 && post.scheduledTime <= currentTime) {
            std::cout << "Scheduled post is now going live!\n";
            postToPlatform(post, "Generic Platform");
            post.scheduledTime = 0;
        }
    }
}

void PostManager::postToPlatform(const Post& post, const std::string& platform) const {
    auto tokenOpt = credentialManager.getToken(platform);
    if (!tokenOpt) {
        ErrorHandler::logError("No token found for " + platform + ". Skipping.", "postToPlatform");
        return;
    }
    std::string token = *tokenOpt;

    SocialMediaCurl curl(token);
    bool success = false;

    if (platform == "twitter") {
        success = curl.postToTwitter(post.message, post.videoPath);
    } else if (platform == "facebook") {
        success = curl.postToFacebook(post.message, post.videoPath);
    } else if (platform == "instagram") {
        success = curl.postToInstagram(post.message, post.videoPath);
    } else if (platform == "tiktok") {
        success = curl.postToTikTok(post.message, post.videoPath);
    } else if (platform == "youtube") {
        success = curl.postToYouTube(post.message, post.videoPath);
    }

    if (success) {
        std::cout << "✅ Successfully posted to " << platform << "!\n";
    } else {
        ErrorHandler::logError("Failed to post to " + platform + ".", "postToPlatform");
    }
}

void PostManager::postToSocialMedia() {
    if (posts.empty()) {
        ErrorHandler::logError("No posts to send.", "postToSocialMedia");
        return;
    }

    for (const auto& post : posts) {
        std::cout << "\nPosting: " << post.message << "\n";
        postToPlatform(post, "twitter");
        postToPlatform(post, "facebook");
        postToPlatform(post, "instagram");
        postToPlatform(post, "tiktok");
        postToPlatform(post, "youtube");
    }
}

void PostManager::clearInputStream() const {
    if (!std::cin.eof()) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

void PostManager::setupToken() {
    std::string platform, token;
    clearInputStream();
    std::cout << "Enter platform name: ";
    std::getline(std::cin, platform);
    std::cout << "Enter token: ";
    std::getline(std::cin, token);

    if (credentialManager.storeToken(platform, token)) {
        std::cout << "✅ Token stored successfully.\n";
    } else {
        ErrorHandler::logError("Failed to store token.", "setupToken");
    }
}
