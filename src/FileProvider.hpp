#pragma once

#include "dmon.hpp"

struct Logger;

namespace aa
{
struct CurrentFileProvider
{
    // FileProvider is an abstraction that allows us to swap out/improve how
    // and when we decide to poll for AA json files.
    // For now, we are only thinking about 1 player worlds.
    CurrentFileProvider();

    dmon::Watch watcher;

    std::optional<std::string> poll(uint64_t ticks = 0);

    void debug();

private:
    Logger* logger;

    // Should these defaults all be in like, DEFAULTS.hpp
    // so they can be properly documented/referred to?
    // e.g. /* poll_interval */ DEFAULT_POLL_INTERVAL
    uint64_t poll_interval = 60;
};
}  // namespace aa
