#include "FileProvider.hpp"

#include "ConfigProvider.hpp"
#include "app_finder.hpp"
#include "dmon.hpp"
#include "logging.hpp"

aa::CurrentFileProvider::CurrentFileProvider()
    : logger(/* why a pointer? */ &get_logger("CurrentFileProvider"))
{
    using aa::conf::get_or;

    // We need to know how often to poll. By default, it's once every 60 ticks.
    auto c        = aa::conf::get();
    poll_interval = get_or(c, "poll_interval", poll_interval);
    if (poll_interval == 0)
    {
        logger->fatal_error("Invalid poll_interval of 0. Must be 1 or above.");
    }

    // Instance based configuration.
    auto conf = aa::conf::getNS("instance");
    if (conf.empty() or not conf.contains("saves"))
    {
        logger->fatal_error(
            "Current implementation requires instance[saves] to be set in configuration.");
    }

    watcher = dmon::Manager::instance().add_watch(conf["saves"].get<std::string>());
}

std::optional<std::string> aa::CurrentFileProvider::poll(uint64_t ticks)
{
    // For now, let's make this function our entire abstraction.
    // We return SOME(str) IFF we need to reset our current advancement state.
    // So: If the active window has changed; if the active world has changed;
    // if the active advancements json has changed.
    // We must track these facts individually and return the appropriate file.

    // I think checking for a changed watcher should always be checked.
    // But, if we have a new focused window, then we should ignore the changed
    // watcher. BUT, that's a super rare race condition, and it's not a big deal?
    // Actually, I'm not sure it's actually that rare of a race condition.
    if (ticks % poll_interval) {
        return {};
    }
    if (const auto changed = watcher.get_change(); changed.has_value())
    {
        // the (saves) directory being watched has updates :)
        logger->debug("watcher with changes: ", watcher.dir_, " of: ", changed.value());
        return changed;
    }
    return watcher.get_change();
}

void aa::CurrentFileProvider::debug() { watcher.debug(); }
