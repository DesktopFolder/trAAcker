#pragma once

#include <string>
#include <optional>

struct Advancement {
    std::optional<std::string> id;
    std::optional<std::string> name;
    std::optional<std::string> icon;
    std::optional<std::string> type;
    std::optional<std::string> short_name;
};
