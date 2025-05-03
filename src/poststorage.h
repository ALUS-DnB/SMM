#ifndef POST_STORAGE_H
#define POST_STORAGE_H

#include "post.h"
#include <vector>
#include <string>

namespace PostStorage {
    bool savePosts(const std::vector<Post>& posts, const std::string& filename);
    bool loadPosts(std::vector<Post>& posts, const std::string& filename);
}

#endif
