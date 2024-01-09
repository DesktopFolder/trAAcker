#pragma once

#include "dmon.hpp"

#include <unordered_map>
#include <vector>
#include <string>

struct Logger;

namespace aa
{
enum class DetectionMode {
    Automatic,
    Explicit,
};

struct CurrentFileProvider
{
    // FileProvider is an abstraction that allows us to swap out/improve how
    // and when we decide to poll for AA json files.
    // For now, we are only thinking about 1 player worlds.
    CurrentFileProvider();

    std::optional<std::string> poll(uint64_t ticks = 0);

    void debug();

    bool has_active_watch() const noexcept
    {
        return not active_watch.empty();
    }

    // Note: It's very easy to modify these watchers...
    // Please be careful with this API.
    // ONLY call if there is an active watch.
    dmon::Watch& get_active_watch()
    {
        if (not has_active_watch())
        {
            logger->fatal_error("Active watch does not exist.");
        }
        return watches.at(active_watch);
    }

private:
    Logger* logger;

    std::vector<std::string> instances;
    DetectionMode mode = DetectionMode::Automatic;

    std::unordered_map<std::string, dmon::Watch> watches;
    std::string active_watch;
    std::string active_instance;

    // Should these defaults all be in like, DEFAULTS.hpp
    // so they can be properly documented/referred to?
    // e.g. /* poll_interval */ DEFAULT_POLL_INTERVAL
    uint64_t poll_interval = 60;
};
}  // namespace aa
