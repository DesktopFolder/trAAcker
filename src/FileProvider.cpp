#include "FileProvider.hpp"

#include "ConfigProvider.hpp"
#include "dmon.hpp"
#include "logging.hpp"

aa::CurrentFileProvider::CurrentFileProvider()
{
    // configuration based implementation
    auto conf = aa::conf::getNS("instance");

    if (conf.empty() or not conf.contains("saves"))
        throw std::runtime_error(
            "Current implementation requires instance.saves to be set in configuration.");

    watcher = dmon::Manager::instance().add_watch(conf["saves"].get<std::string>());
}

std::optional<std::string> aa::CurrentFileProvider::poll()
{
    if (const auto changed = watcher.get_change(); changed.has_value())
    {
        // the (saves) directory being watched has updates :)
        get_logger("FileProvider")
            .debug("watcher with changes: ", watcher.dir_, " of: ", changed.value());
    }
    return watcher.get_change();
}
