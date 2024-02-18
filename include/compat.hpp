#pragma once

#include <string>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#define TRAACKER_WINDOWS_BUILD
#elif __APPLE__
#define TRAACKER_MACOS_BUILD
#elif __linux__
#define TRAACKER_LINUX_BUILD
#else
#error "Unsupported OS? D:"
#endif

namespace aa {
void normalize_path(std::string&);
}