#include "dmon.hpp"

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

    reinterpret_cast<dmon::impl::WatchData*>(user)->check_change(filepath);
}

dmon::Watch::~Watch() { deactivate(); }

dmon::Watch::Watch(std::string dirname, std::string filename)
    : dir_(dirname)
{
    data_ = std::make_unique<impl::WatchData>(filename);
    id_ = dmon_watch(dirname.c_str(), watch_callback, 0, data_.get()).id;
}

void dmon::Watch::deactivate()
{
    if (id_.has_value()) dmon_unwatch(dmon_watch_id{*id_});
}

dmon::Manager::Manager() { dmon_init(); }
dmon::Manager::~Manager() { dmon_deinit(); }
