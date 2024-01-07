#pragma once

#include "ApplicationInfo.hpp"

#include <optional>

namespace sf {
    class RenderWindow;
}

ApplicationInfo get_focused_application(sf::RenderWindow&);

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
// Windows specific code for getting the current application details.

#elif __APPLE__
// Apple specific code for getting the current application details.
#include "osx/FocusedApplicationFinder.hpp"

inline ApplicationInfo get_focused_application(sf::RenderWindow&)
{
    return osx::get_current_application();
}

inline std::optional<bool> get_focused_minecraft(sf::RenderWindow& x)
{
    const auto s = get_focused_application(x);

    if (not s.exec.starts_with("java")) return {};
    return true;
}

#elif __linux__
// Linux specific code for getting the current application details.
#else
#   error "Unsupported OS? D:"
#endif
