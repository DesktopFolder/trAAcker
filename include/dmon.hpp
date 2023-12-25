#pragma once

#include <atomic>
#include <cassert>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <string_view>
#include <utility>

#include "logging.hpp"

namespace dmon
{
using watch_id = uint32_t;

namespace impl
{
struct WatchData
{
    WatchData() = default;
    WatchData(std::string watch) : file_watched(watch) {}

    std::atomic<bool> has_modifications;
    std::string file_watched;

    std::mutex change_guard;
    std::string change;

    void check_change(std::string_view file_path)
    {
        const auto& logger = get_logger("dmon");
        logger.debug("Watching '", file_watched, "', found ", file_path);
        if (file_watched.empty() || file_path == file_watched)
        {
            logger.debug("Storing the fact that we found changes.");
            std::lock_guard l(change_guard);
            change = file_path;
            has_modifications.store(true);
        }
    }
};
} // namespace impl

struct Watch
{
    friend class Manager;

    // Movement and deletion constructors.
    // Handles empty ids (not constructed, etc)
    Watch(Watch&& other)
        : id_(std::exchange(other.id_, {})), dir_(std::move(other.dir_)),
          data_{std::move(other.data_)}
    {
    }

    Watch& operator=(Watch&& other)
    {
        id_   = std::exchange(other.id_, {});
        dir_  = std::move(other.dir_);
        data_ = std::move(other.data_);
        return *this;
    }

    ~Watch();

    Watch() = default;

    bool is_active() { return id_.has_value(); }
    bool has_changes()
    {
        if (data_) return data_->has_modifications.load();
        return false;
    }

    std::optional<std::string> get_change()
    {
        if (!data_) return std::nullopt;
        if (data_->has_modifications.load())
        {
            std::lock_guard l(data_->change_guard);
            data_->has_modifications.store(false);
            return data_->change;
        }
        return std::nullopt;
    }

    void deactivate();

    std::string dir_; // don't modify this :)

private:
    Watch(std::string dirname, std::string filename = "");

    std::unique_ptr<impl::WatchData> data_;

    std::optional<watch_id> id_;
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
    Watch add_filewatch(std::string dirname, std::string filename)
    {
        return Watch(dirname, filename);
    }

private:
    Manager();
    ~Manager();
};
} // namespace dmon
