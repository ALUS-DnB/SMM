#include "post.h"

#include <algorithm>

Post::Post(const std::string& platform, const std::string& content)
    : platform(platform), content(content) {}

std::string Post::getContent() const {
    return content;
}

void Post::setContent(const std::string& content) {
    this->content = content;
}

std::string Post::getPlatform() const {
    return platform;
}

void Post::setPlatform(const std::string& platform) {
    this->platform = platform;
}

bool Post::addTag(const std::string& tag) {
    // Check if tag already exists before adding
    if (std::find(tags.begin(), tags.end(), tag) == tags.end()) {
        tags.push_back(tag);
        return true;
    } else {
        // Log a warning instead of an error
        ErrorHandler::logError("Tag already exists", "addTag");
        return false;
    }
}

bool Post::removeTag(const std::string& tag) {
    auto it = std::find(tags.begin(), tags.end(), tag);
    if (it != tags.end()) {
        tags.erase(it);
        return true;
    } else {
        // Log a warning instead of an error
        ErrorHandler::logError("Tag not found", "removeTag");
        return false;
    }
}

bool Post::editTag(const std::string& oldTag, const std::string& newTag) {
    auto it = std::find(tags.begin(), tags.end(), oldTag);
    if (it != tags.end()) {
        *it = newTag;
        return true;
    } else {
        // Log a warning instead of an error
        ErrorHandler::logError("Old tag not found", "editTag");
        return false;
    }
}

const std::vector<std::string>& Post::getTags() const {
    return tags;
}
