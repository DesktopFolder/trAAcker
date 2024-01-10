#include "FileProvider.hpp"

#include "ConfigProvider.hpp"
#include "app_finder.hpp"
#include "dmon.hpp"
#include "logging.hpp"

#include <filesystem>

std::optional<std::string> most_recent_advancement_dir(std::string_view instance_path)
{
    namespace fs = std::filesystem;

    fs::path saves_path{instance_path};
    saves_path /= ".minecraft";
    saves_path /= "saves";
    if (not fs::is_directory(saves_path))
    {
        log::debug("Not directory: ", saves_path);
        return std::nullopt;
    }

    fs::file_time_type top_time{};
    fs::path top_path;
    for (auto& dir_entry : fs::directory_iterator{saves_path})
    {
        const auto new_time = fs::last_write_time(dir_entry.path());
        if (new_time > top_time)
        {
            top_time = new_time;
            // I dunno if this works but hey!
            top_path = std::move(dir_entry.path());
        }
    }

    if (top_path.empty())
    {
        log::debug("Did not find any saves in ", saves_path);
        return std::nullopt;
    }

    auto adv_dir = top_path / "advancements";
    if (fs::is_directory(adv_dir))
    {
        return adv_dir;
    }
    log::debug("Not directory: ", adv_dir);
    return std::nullopt;
}

std::optional<std::string> most_recent_advancements(std::string_view advancements_path)
{
    namespace fs = std::filesystem;

    fs::file_time_type top_time{};
    fs::path top_path;
    for (auto& dir_entry : fs::directory_iterator{advancements_path})
    {
        if (dir_entry.path().extension() != ".json")
        {
            // Ignore non-JSON files.
            continue;
        }

        const auto new_time = fs::last_write_time(dir_entry.path());
        if (new_time > top_time)
        {
            top_time = new_time;
            // I dunno if this works but hey!
            top_path = std::move(dir_entry.path());
        }
    }

    return top_path;
}

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

    std::string current_instance = [&]()
    {
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
                if (std::any_of(instances.begin(), instances.end(),
                                [&focused](const std::string& name) { return name == focused; }))
                {
                    return std::move(*fm);
                }

                // We have an instance open that isn't one of the ones we watch.
                logger->info("Ignoring focused Minecraft instance: ", *fm, ", as it is",
                             " not one of the instances listed in \"instances\".");
                return std::string{""};
            }
            // logger->debug("No minecraft instance found.");
            // We don't currently have an instance open.
            return std::string{""};
        }
        else /* A single manual instance */
        {
            // Guaranteed to be valid.
            // We have no way to know if it's focused or not, so we always auto-update.
            return instances[0];
        }
    }();

    // No instances have been opened/focused ever.
    if (current_instance.empty() and active_instance.empty())
    {
        return std::nullopt;
    }

    if (current_instance.empty())
    {
        // Default current instance = active instance
        current_instance = active_instance;
    }

    // Let's say the user changes to an instance that isn't valid.
    // Then we want everything to stay the same.
    std::string current_advancement_dir;
    if (auto cur = most_recent_advancement_dir(current_instance); cur.has_value())
    {
        // logger->debug("Found advancement directory: ", *cur);
        current_advancement_dir = std::move(*cur);
    }
    else
    {
        // We couldn't find a current advancement dir for the current instance.
        // That means that we need to fall back and use the old one.
        // Instead of returning here - because we still want to update if the
        // fallback (active) instance has an update. Might as well.

        // Note: It is possible for our active advancements directory to be empty.
        // This would occur if:
        // - We have never recognized a valid instance
        // - We get a current instance
        // - The current instance does not have any worlds or the latest one is invalid.
        // In that case, return here.
        if (active_advancement_dir.empty())
        {
            return std::nullopt;
        }
        current_advancement_dir = active_advancement_dir;
    }

    // Guaranteed: We have a current instance.
    // Either from when we put a watch on a file previous,
    // or from right now, when we found a new instance.

    // Guaranteed: We have a current advancement directory.
    // Either from when we put a watch on a file previously,
    // or from right now, when we found a new directory.

    // Now if we have a new advancements dir, we set a watch on it.
    // Otherwise, we check our current watch to see if it has changed.
    std::string current_advancements_file;
    if (current_advancement_dir != active_advancement_dir || not has_active_watch())
    {
        // New advancements directory. Get the most recently changed file,
        // then set a watch on the folder.
        if (auto cur = most_recent_advancements(current_advancement_dir); cur.has_value())
        {
            active_watch = dmon::Manager::instance().add_watch(current_advancement_dir);

            // Okay, we have a file. This is the file we're watching now, so:
            active_advancements    = std::move(*cur);
            active_advancement_dir = std::move(current_advancement_dir);

            // This might do nothing.
            active_instance = std::move(current_instance);

            logger->debug("Found a new advancements directory: ", active_advancement_dir);
            return active_advancements;
        }
        // We either don't have an active watch, or our advancement directory changed.
        else if (not has_active_watch())
        {
            // We don't have an active watch. And we also could not get the current
            // advancements directory...
            // Basically, current_advancement_dir doesn't contain anything useful.
            // And we don't have anything active. So there is nothing to do.
            logger->debug("No watch + no current advancements directory, could not reset.");
            return std::nullopt;
        }
        else
        {
            // We have an active watch, but our current directory isn't the same as our
            // active one. But, our active one doesn't have a JSON file.
            // So, use our current watch to check for updates.
            if (auto changed = active_watch.get_change(); changed.has_value())
            {
                // the (saves) directory being watched has updates :)
                logger->debug("Active watcher with changes: ", active_watch.dir_,
                              " of: ", changed.value());
                // We DON'T want to update any of our active data. Because that's still
                // what we have active! So just return the file that was updated.
                return std::move(*changed);
            }
            else
            {
                // No updates from our watch. Therefore, nothing to worry about.
                return std::nullopt;
            }
        }
    }
    else
    {
        // We haven't moved around, AND our watch exists.
        // Check if we have an update from our watch.
        if (auto changed = active_watch.get_change(); changed.has_value())
        {
            // the (saves) directory being watched has updates :)
            logger->debug("watcher with changes: ", active_watch.dir_, " of: ", changed.value());
            active_advancements = std::move(*changed);
            return active_advancements;
        }
        else
        {
            // Nope, no changes!
            return std::nullopt;
        }
    }
}

void aa::CurrentFileProvider::debug() { get_active_watch().debug(); }
