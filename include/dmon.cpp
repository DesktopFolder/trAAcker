#include "dmon.hpp"

#include "logging.hpp"

// todo - just ignore everything ever here.
#ifndef _WIN32
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc++11-narrowing"
#endif
#define DMON_IMPL
#include "dmon.h"
#ifndef _WIN32
#pragma clang diagnostic pop
#endif

namespace aa
{
// Utility functions - mostly enum conversion etc
std::string action_to_string(dmon_action action)
{
    switch (action)
    {
    case DMON_ACTION_CREATE: return "CREATE";
    case DMON_ACTION_DELETE: return "DELETE";
    case DMON_ACTION_MODIFY: return "MODIFY";
    case DMON_ACTION_MOVE: return "MOVE";
    default: assert(false);
    }
}

void watch_callback(dmon_watch_id watch_id, dmon_action action, const char* rootdir,
                    const char* filepath, const char* oldfilepath, void* user)
{
    // Debugging / setup things.
    // std::cout << "Begin watch callback. Data:" << std::endl;
    // std::cout << action_to_string(action) << std::endl;
    // std::cout << filepath << std::endl;
    // if (oldfilepath == nullptr)
    //{
    //    std::cout << "Old file path was nullptr." << std::endl;
    //}
    // else
    //{
    //    std::cout << oldfilepath << std::endl;
    //}
    // std::cout << "End of data." << std::endl;
    if (action == DMON_ACTION_DELETE)
    {
        // Ignore files that are deleted... obviously. For now. We could
        // fix our custom callback system later to use more info, but...
        return;
    }

    log::debug("DMON watch_callback got callback in: ", rootdir, ", with ", filepath);

    reinterpret_cast<dmon::impl::WatchData*>(user)->check_change(filepath);
}

dmon::Watch::~Watch() { deactivate(); }

dmon::Watch::Watch(std::string dirname, std::string filename) : dir_(dirname)
{
    // A little bit cursed, but if it works...
    if (not dir_.ends_with("/"))
    {
        dir_ += "/";
    }
    data_ = std::make_unique<impl::WatchData>(filename);
    id_   = dmon_watch(dir_.c_str(), watch_callback, 0, data_.get()).id;
    get_logger("dmon::Watch").debug("Created dmon watch of directory: ", dir_, " with ID: ", *id_);
}

// way safer way to use this shit... lol.
void dmon::Watch::set_to(std::string dirname)
{
    if (!data_)
    {
        // A little bit cursed, but if it works...
        if (not dirname.ends_with("/"))
        {
            dirname += "/";
        }
        dir_ = dirname;
        // TODO - do we ever want to watch a specific file? ehhh
        // maybe should do that outside of here or in a separate version
        data_ = std::make_unique<impl::WatchData>("");
        id_   = dmon_watch(dir_.c_str(), watch_callback, 0, data_.get()).id;
        return;
    }
    else
    {
        std::lock_guard l(data_->change_guard);
    }
}

void dmon::Watch::debug() const
{
    const auto& logger = get_logger("dmon::Watch");
    logger.debug("Debugging Watch for directory: ", dir_);
    logger.debug("Do we have file data? ", data_ ? "Yes" : "No");
    logger.debug("Do we have a valid watch? ", id_ ? "Yes" : "No");
    if (data_)
    {
        logger.debug("Data has a modification? ", data_->has_modifications.load() ? "Yes" : "No");
    }
    if (id_)
    {
        logger.debug("Watch ID: ", *id_);
    }
}

void dmon::Watch::deactivate()
{
    if (id_.has_value())
    {
        dmon_unwatch(dmon_watch_id{*id_});
        log::debug("no more id: ", *id_);
        id_.reset();
    }
}

dmon::Manager::Manager() { dmon_init(); }
dmon::Manager::~Manager() { dmon_deinit(); }
} // namespace aa