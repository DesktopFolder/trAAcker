#pragma once

#include "ApplicationInfo.hpp"
#include <optional>
#include <string_view>

namespace aa {
constexpr std::string_view MMC_LIB_PATH_PREFIX="-Djava.library.path=";

// returns an app info object but unused idk, don't bother with this LOL!
ApplicationInfo get_focused_application();

// returns the path to some part of the instance directory, not sure where :) gl
std::optional<std::string> get_focused_minecraft();
}