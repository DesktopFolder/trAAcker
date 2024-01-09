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

    /** Pipeline information:
     * Level 1: Instance detection.
     * Level 2: World detection.
     * Level 3: Save watching.
     *
     * Level 1 is the only one that needs to be configurable, so that
     * the user can choose between "manually listing instances" and
     * "automatically detecting open instances".
     */

    // Instance based configuration.
    instances = aa::conf::get_or<std::vector<std::string>>(c, "instances", {});
    for (auto& inst : instances)
    {
    }

    if (instances.empty())
    {
        logger->debug("Choosing to autodetect instances.");
    }
    else
    {
        logger->debug("Choosing to use an explicit list of instances.");
        mode = DetectionMode::Explicit;
    }
    // watcher = dmon::Manager::instance().add_watch(conf["saves"].get<std::string>());
}

std::optional<std::string> aa::CurrentFileProvider::poll(uint64_t ticks)
{
    // Quick return at the top here before we do anything else.
    if (ticks % poll_interval)
    {
        return {};
    }

    // For now, let's make this function our entire abstraction.
    // We return SOME(str) IFF we need to reset our current advancement state.
    // So: If the active window has changed; if the active world has changed;
    // if the active advancements json has changed.
    // We must track these facts individually and return the appropriate file.

    // I think checking for a changed watcher should always be done.
    // But, if we have a new focused window, then we should ignore the changed
    // watcher. BUT, that's a super rare race condition, and it's not a big deal?
    // Actually, I'm not sure it's actually that rare of a race condition.
    //
    // Imagine the following use case: The user resets from I1 to I2. I1 is written,
    // but I2 is our new active instance. Should we parse the update in I1?
    //
    // I think generally the AVERAGE performance impact of parsing the I1 update
    // uselessly is not that big of a deal. Especially considering it would be a full
    // second (well, depending on the poll interval, framerate, etc) before the
    // parse was ran for the second time.
    //
    // However, in the case where this is expected to actually happen, a new world has
    // been loaded into, so it's two full reparses/reloads in the span of two seconds
    // while world generation is going on. That seems a little bit more problematic
    // (even though the actual cost of those reloads should be... extremely negligible)
    //
    // I think we will poll first. However, as a quick TODO: Investigate the
    // actual runtime performance impact of the polling function, parsing function, etc.
    
    std::string current_instance = [&]() {
    if (mode == DetectionMode::Automatic || instances.size() > 1)
    {
        // Get the currently active instance.
        if (const auto fm = get_focused_minecraft(); fm.has_value())
        {
            if (mode == DetectionMode::Automatic)
            {
                return std::move(*fm);
            }

            // If we aren't in autodetection mode, that means we have 2+ instances.
            // Only autodetect if THOSE windows are open.
            const auto& focused = *fm;
            if (std::any_of(instances.begin(), instances.end(), [&focused](const std::string& name) { return name == focused; }))
            {
                return std::move(*fm);
            }

            // We have an instance open that isn't one of the ones we watch.
            logger->debug("Ignoring focused Minecraft instance: ", *fm, ", as it is",
                          " not one of the instances listed in \"instances\".");
            return std::string{""};
        }
        // We don't currently have an instance open.
        return std::string{""};
    }
    else /* A single manual instance */ {
        // Guaranteed to be valid.
        // We have no way to know if it's focused or not, so we always auto-update.
        return instances[0]; 
    }
    }();

    // Instance changed IFF we got a new instance.
    const bool instance_changed = (not current_instance.empty() and current_instance != active_instance);

    if (!instance_changed)
    {
        // We are still on the same instance as before. We need to check if we have a world now that we
        // didn't before (a new world, an updated one, etc...)
    }

    // If we don't have an active watch, just return.
    if (not has_active_watch())
    {
        return std::nullopt;
    }

    auto& active = get_active_watch();
    if (const auto changed = active.get_change(); changed.has_value())
    {
        // the (saves) directory being watched has updates :)
        logger->debug("watcher with changes: ", active.dir_, " of: ", changed.value());
        return changed;
    }
    return std::nullopt;
}

void aa::CurrentFileProvider::debug() { get_active_watch().debug(); }
