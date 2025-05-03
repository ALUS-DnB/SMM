#include "post_storage.h"
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace PostStorage {

    static json postToJson(const Post& post) {
        return {
            {"message", post.message},
            {"videoPath", post.videoPath},
            {"tags", post.getTags()},
            {"scheduledTime", post.scheduledTime}
        };
    }

    static Post jsonToPost(const json& j) {
        Post post(j.value("message", ""), j.value("videoPath", ""));
        post.scheduledTime = j.value("scheduledTime", 0);
        for (const auto& tag : j["tags"]) {
            post.addTag(tag);
        }
        return post;
    }

    bool savePosts(const std::vector<Post>& posts, const std::string& filename) {
        json j;
        for (const auto& post : posts) {
            j.push_back(postToJson(post));
        }

        std::ofstream out(filename);
        if (!out.is_open()) return false;

        out << j.dump(4);
        return true;
    }

    bool loadPosts(std::vector<Post>& posts, const std::string& filename) {
        std::ifstream in(filename);
        if (!in.is_open()) return false;

        json j;
        in >> j;

        posts.clear();
        for (const auto& item : j) {
            posts.push_back(jsonToPost(item));
        }

        return true;
    }
}
