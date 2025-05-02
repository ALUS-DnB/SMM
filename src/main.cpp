#include "credentialmanager.h"
#include "postmanager.h"

#include <iostream>
#include <string>

int main() {
    PostManager postManager;

    std::cout << "🔐 Tip: Use option 8 in the menu to set up tokens securely for each platform.\n";

    postManager.run();

    return 0;
}
