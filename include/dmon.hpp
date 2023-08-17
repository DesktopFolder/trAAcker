#pragma once

#include <atomic>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>
#include <utility>

// todo - just ignore everything ever here.
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc++11-narrowing"
#define DMON_IMPL
#include "dmon.h"
#pragma clang diagnostic pop

namespace dmon {
namespace impl {
// Utility functions - mostly enum conversion etc
std::string action_to_string(dmon_action action) {
    switch (action) {
        case DMON_ACTION_CREATE:
            return "CREATE";
        case DMON_ACTION_DELETE:
            return "DELETE";
        case DMON_ACTION_MODIFY:
            return "MODIFY";
        case DMON_ACTION_MOVE:
            return "MOVE";
        default:
            assert(false);
    }
}

struct WatchData {
    std::atomic<bool> has_modifier;
    const std::string file_watched;

    void check_change(std::string_view file_path) {
        // std::cout << "Watching " << file_watched << ", found " << file_path << std::endl;
        if (file_watched.empty() || file_path == file_watched) {
            // std::cout << "Stored changes!" << std::endl;
            has_modifier.store(true);
        }
    }
};

static void watch_callback(dmon_watch_id watch_id, dmon_action action, const char* rootdir, const char* filepath,
                           const char* oldfilepath, void* user) {
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

    reinterpret_cast<WatchData*>(user)->check_change(filepath);
}
}  // namespace impl

struct Watch {
    friend class Manager;

    std::string dir_;
    impl::WatchData data_{false, ""};
    std::optional<dmon_watch_id> id_;

    Watch(Watch&& other) : id_(std::exchange(other.id_, {})), dir_(std::move(other.dir_)) {}
    ~Watch() {
        if (!id_) dmon_unwatch(*id_);
    }

   private:
    Watch(std::string dirname, std::string filename = "") : dir_(dirname), data_{false, filename} {
        id_ = dmon_watch(dirname.c_str(), impl::watch_callback, 0, &data_);
    }
};

// This should be a singleton, but ehhhh
struct Manager {
    static Manager& instance() {
        static Manager m;
        return m;
    }

    Watch add_watch(std::string dirname) { return Watch(dirname); }
    Watch add_filewatch(std::string dirname, std::string filename) { return Watch(dirname, filename); }

   private:
    Manager() { dmon_init(); }
    ~Manager() { dmon_deinit(); }
};
}  // namespace dmon
