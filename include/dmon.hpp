#pragma once

#include <atomic>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>
#include <utility>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc++11-narrowing"
#define DMON_IMPL
#include "dmon.h"
#pragma clang diagnostic pop

namespace dmon
{
namespace impl
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
static void watch_callback(dmon_watch_id watch_id, dmon_action action, const char* rootdir,
                           const char* filepath, const char* oldfilepath, void* user)
{
    // Debugging / setup things.
    std::cout << "Begin watch callback. Data:" << std::endl;
    std::cout << action_to_string(action) << std::endl;
    std::cout << filepath << std::endl;
    if (oldfilepath == nullptr)
    {
        std::cout << "Old file path was nullptr." << std::endl;
    }
    else
    {
        std::cout << oldfilepath << std::endl;
    }
    std::cout << "End of data." << std::endl;

    reinterpret_cast<std::atomic<bool>*>(user)->store(true);
}
}  // namespace impl

struct Watch
{
    friend class Manager;

    std::string dir_;
    std::atomic<bool> triggered_;
    std::optional<dmon_watch_id> id_;

    Watch(Watch&& other) : id_(std::exchange(other.id_, {})), dir_(std::move(other.dir_)) {}
    ~Watch() { if (!id_) dmon_unwatch(*id_); }

private:
    Watch(std::string dirname) : dir_(dirname)
    {
        id_ = dmon_watch(dirname.c_str(), impl::watch_callback, 0, &triggered_);
    }
};

// This should be a singleton, but ehhhh
struct Manager
{
    static Manager& instance()
    {
        static Manager m;
        return m;
    }

    Watch add_watch(std::string dirname) { return Watch(dirname); }

private:
    Manager() { dmon_init(); }
    ~Manager() { dmon_deinit(); }
};
}  // namespace dmon
