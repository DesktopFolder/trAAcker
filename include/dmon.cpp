#include "dmon.hpp"

#include "logging.hpp"

// todo - just ignore everything ever here.
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc++11-narrowing"
#define DMON_IMPL
#include "dmon.h"
#pragma clang diagnostic pop

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
    log::debug("callback from ID: ", watch_id.id);

    // LMAO. This does not work. As what if the watch data has been removed/deleted?
    // Of course, that is nonfunctional. So we have a race condition around here-ish,
    // which needs to be fixed. Can't believe I already ran into this within like 5
    // minutes of testing this out. I don't understand how that is even POSSIBLE.
    // Probably something obvious I'm missing, of course.
    // Okay, the obvious fix to this is pretty obvious. We should only have one watch
    // object, and never actually 'delete' the object, simply change what directory
    // we watch. If we get spurious callbacks for old directories, simply ignore them.
    reinterpret_cast<dmon::impl::WatchData*>(user)->check_change(filepath);
}

dmon::Watch::~Watch()
{ 
    log::debug("Destroying watch.");
    if (id_.has_value())
    {
        log::debug("with id ID: ", *id_);
    }
    deactivate();
}

dmon::Watch::Watch(std::string dirname, std::string filename) : dir_(dirname)
{
    // A little bit cursed, but if it works...
    if (not dir_.ends_with("/"))
    {
        dir_ += "/";
    }
    data_ = std::make_unique<impl::WatchData>(filename);
    id_   = dmon_watch(dir_.c_str(), watch_callback, 0, data_.get()).id;
    if (id_.has_value())
    {
        log::debug("activating ID: ", *id_);
    }
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
}

void dmon::Watch::deactivate()
{
    if (id_.has_value())
    {
        log::debug("deactivating ID: ", *id_);
        dmon_unwatch(dmon_watch_id{*id_});
    }
}

dmon::Manager::Manager() { dmon_init(); }
dmon::Manager::~Manager() { dmon_deinit(); }
