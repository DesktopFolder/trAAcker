#include "FileProvider.hpp"

#include "ConfigProvider.hpp"
#include "dmon.hpp"
#include "logging.hpp"

aa::CurrentFileProvider::CurrentFileProvider() : logger(&get_logger("CurrentFileProvider"))
{
    // configuration based implementation
    auto conf = aa::conf::getNS("instance");

    if (conf.empty() or not conf.contains("saves"))
    {
        logger->fatal_error(
            "Current implementation requires instance[saves] to be set in configuration.");
    }

    watcher = dmon::Manager::instance().add_watch(conf["saves"].get<std::string>());
}

std::optional<std::string> aa::CurrentFileProvider::poll()
{
    if (const auto changed = watcher.get_change(); changed.has_value())
    {
        // the (saves) directory being watched has updates :)
        logger->debug("watcher with changes: ", watcher.dir_, " of: ", changed.value());
        return changed;
    }
    return watcher.get_change();
}

void aa::CurrentFileProvider::debug() { watcher.debug(); }
