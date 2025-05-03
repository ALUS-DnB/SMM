#ifndef POST_MANAGER_H
#define POST_MANAGER_H

#include "post.h"
#include "socialmediacurl.h"
#include "errorhandler.h"
#include "credentialmanager.h"
#include "post_storage.h"

#include <vector>
#include <string>

class PostManager {
public:
    void run();

private:
    std::vector<Post> posts;
    CredentialManager credentialManager;

    static constexpr int MAX_CHAR_LIMIT = 280;
    static constexpr const char* SAVE_FILE = "saved_posts.json";

    void showMenu() const;
    void handleChoice(int choice);

    void createPost();
    void viewPosts() const;
    void editPostByIndex();

    void postToSocialMedia();
    void postToPlatform(const Post& post, const std::string& platform) const;

    void schedulePostMenu();
    void schedulePost(Post& post, int delayInSeconds);
    void handleScheduledPosts();

    void setupToken();

    void logAction(const std::string& actionMessage) const;
    void logError(const std::string& errorMessage) const;
    void clearInputStream() const;

    void savePostsToFile() const;
    void loadPostsFromFile();

    void archivePostedPosts();
    void chooseAndPostSingle();
};

#endif
