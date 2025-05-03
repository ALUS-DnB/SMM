# 📣 Social Media Manager

A cross-platform C++ application designed to create, schedule, and publish posts to multiple social media platforms.

## 🚀 Features

- Compose posts with optional media attachments and hashtags
- View, edit, and manage your post library
- Schedule posts for future publication
- Publish to platforms like Twitter, Facebook, Instagram, TikTok, and YouTube
- Manage API tokens securely
- Cross-platform builds: Linux, Windows, and macOS

## 🗂 Project Structure

```
├── src/
│   ├── main.cpp
│   ├── postmanager.cpp
│   ├── postmanager.h
│   ├── post.cpp
│   ├── post.h
│   ├── socialmediacurl.cpp
│   ├── socialmediacurl.h
│   ├── credentialmanager.cpp
│   ├── credentialmanager.h
│   ├── errorhandler.cpp
│   ├── errorhandler.h
│   └── include/
│   	└── nlohmann/
│   		└── json.hpp
├── Makefile
├── README.md
└── LICENSE
```

## 🛠️ Build Instructions

### Prerequisites

Ensure the following are installed:

- C++17 compatible compiler (e.g., `g++`, `clang++`)
- `libcurl` development libraries
- `libsecret` development libraries
- `make`

### Building on Linux

```bash
sudo apt update
sudo apt install build-essential libcurl4-openssl-dev libsecret-1-dev
make linux
```

### Cross-Compiling for Windows

```bash
sudo apt install mingw-w64
make windows
```

### Cross-Compiling for macOS

Set up a macOS cross-compilation toolchain (e.g., osxcross). Once configured:

```bash
make mac
```

### Building All Targets

```bash
make all
```

## 📦 Usage

Run the application from the command line:

```bash
./SocialMediaManager_linux
```

Follow the on-screen menu to create, view, edit, schedule, and publish posts.

## 🔐 Token Management

To post to social media platforms, you'll need to set up API tokens:

1. Select the "Set up token" option from the main menu.
2. Enter the platform name (e.g., `twitter`).
3. Enter the corresponding API token.

Tokens are stored securely using the platform's credential manager.

## 🧪 Sample Execution

Upon running the application, you might see:

```
=== Social Media Post Manager ===
1. Create a new post
2. View all posts
3. View post by number
4. Edit a post
5. Post to social media platforms
6. Schedule a post
7. Exit
8. Set up token
Choose an option:
```

## 📄 License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.
