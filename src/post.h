#ifndef POST_H
#define POST_H

#include "errorhandler.h"

#include <string>
#include <vector>
#include <optional>

class Post {
public:
    // Constructor
    Post(const std::string& platform, const std::string& content);

    // Getters and setters for content and platform
    std::string getContent() const;
    void setContent(const std::string& content);

    std::string getPlatform() const;
    void setPlatform(const std::string& platform);

    // Methods for tags with return values indicating success/failure
    bool addTag(const std::string& tag);
    bool removeTag(const std::string& tag);
    bool editTag(const std::string& oldTag, const std::string& newTag);
    const std::vector<std::string>& getTags() const;

private:
    std::string content;
    std::string platform;
    std::vector<std::string> tags; // Tags for the post
};

#endif // POST_H
