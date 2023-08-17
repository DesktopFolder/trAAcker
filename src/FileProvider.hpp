#pragma once

#include "dmon.hpp"

namespace aa
{
struct CurrentFileProvider
{
    // FileProvider is an abstraction that allows us to swap out/improve how
    // and when we decide to poll for AA json files.
    // For now, we are only thinking about 1 player worlds.
    CurrentFileProvider();

    dmon::Watch watcher;

    std::optional<std::string> poll();
};
}  // namespace aa
