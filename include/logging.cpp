#include "logging.hpp"

Logger& get_logger(std::string_view name) {
    static string_map<Logger> loggers;
    
    if (not loggers.contains(name))
    {
        loggers.emplace(name, Logger(std::string{name}));
    }

    return loggers.find(name)->second;
}
