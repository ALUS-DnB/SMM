#ifndef POSTMANAGER_H
#define POSTMANAGER_H

#include "post.h"
#include "errorhandler.h"
#include "socialmediacurl.h"
#include "credentialmanager.h"

#include <vector>
#include <string>

class PostManager {
public:
    void run();

private:
    static constexpr std::size_t MAX_CHAR_LIMIT = 280;

    std::vector<Post> posts;
    CredentialManager credentialManager;

    void showMenu() const;
    int getValidatedChoice(int min, int max) const;

    void createPost();
    void viewPosts() const;
    void viewPostByIndex() const;
    void editPostByIndex();
    void handleChoice(int choice);
    void schedulePostMenu();
    void handleScheduledPosts();

    void postToPlatform(const Post& post, const std::string& platform) const;
    void postToSocialMedia();
    void schedulePost(Post& post, int delayInSeconds);

    void setupToken();
    void logAction(const std::string& actionMessage) const;
    void logError(const std::string& errorMessage) const;

    void clearInputStream() const;
};

#endif // POSTMANAGER_H
