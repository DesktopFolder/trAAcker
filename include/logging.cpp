#include "logging.hpp"

namespace aa {
bool Logger::stdout_default = false;
LogLevel Logger::level      = LogLevel::Debug;

string_map<std::unique_ptr<std::ofstream>>& detail::get_files()
{
    static string_map<std::unique_ptr<std::ofstream>> files;
    return files;
}

std::ofstream* get_file(std::string_view name)
{
    // Ugh, these are not safe, because of realloc.
    // We shouldn't store these refs but I really want to for performance.
    // What's the right way to deal with this?
    // Pointers are also super inefficient here. Really unnecessary redirections.

    /* Ugh. Okay. I guess this is something to think about right now.
     * We need to remove the files after we remove the loggers. So, we need
     * to create the log files static map first, then create the map of loggers.
     * Then we just store pointers to files within the loggers. Pretty easy.
     * It looks 'uglier', but ultimately it's the same level of indirection as
     * just having references (nicer but we want nulling + resetting) and we
     * are guaranteed that the objects continue to exist.
     * That solves one problem of static ordering, but DOES NOT SOLVE the
     * problem pointed to in the previous comment, which is that map realloc
     * will immediately ruin all pointers. I really don't want to have to deal
     * with this particular issue. I think what I will do is use unique pointers.
     */
    auto& files = detail::get_files();

    if (not files.contains(name))
    {
        auto [p, _] = files.emplace(name, std::make_unique<std::ofstream>(
                                              std::string{name}, std::ios::trunc | std::ios::out));
        if (not p->second->good())
        {
            log::error("Failed to open log file: ", name);
        }
    }

    auto& fptr = files.find(name)->second;
    if (fptr.get() == nullptr)
    {
        return files.find("__default__")->second.get();
    }

    return fptr.get();
}

Logger& get_logger(std::string_view name)
{
    static string_map<Logger> loggers;

    if (not loggers.contains(name))
    {
        auto [l, _] = loggers.emplace(name, Logger(std::string{name}));
        if (detail::get_files().contains("__default__"))
        {
            l->second.file = get_file("__default__");
        }
    }

    return loggers.find(name)->second;
}

LogLevel Logger::set_level(std::string_view level)
{
    if (level == "debug")
    {
        Logger::level = LogLevel::Debug;
    }
    else if (level == "info")
    {
        Logger::level = LogLevel::Info;
    }
    else if (level == "warning")
    {
        Logger::level = LogLevel::Warning;
    }
    else if (level == "error")
    {
        Logger::level = LogLevel::Error;
    }
    else if (level == "none")
    {
        Logger::level = LogLevel::None;
    }
    else
    {
        get_logger("Logger").fatal_error(
            "Could not parse log level: '", level,
            "'. Valid values are: 'debug', 'info', 'warning', 'error', or 'none'.");
    }

    return Logger::level;
}
}