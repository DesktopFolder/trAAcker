#pragma once

/**
 * logging.hpp
 *
 * An set of abstractions for logging necessities.
 * For now, everything goes in the header.
 */

#include "utilities.hpp"
#include <fstream>
#include <iostream>
#include <optional>

struct Logger
{
    // Not super configurable, but it works for now.
    std::optional<std::ofstream> file;
    bool write_stdout{true};

    Logger(std::string name)
        : name_(std::move(name)), str_debug_("DEBUG (" + name_ + "): "),
          str_warning_("WARNING (" + name_ + "): "), str_error_("ERROR (" + name_ + "): "),
          str_info_("INFO (" + name_ + "): ")
    {
    }

    // Level 0 - Only logged when we are at the most verbose.
    // Anything that would hugely spam the logs should be under this.
    template <typename... Ts>
    void debug(Ts&&... ts)
    {
        write_endl(str_debug_, std::forward<Ts>(ts)...);
    }

    template <typename... Ts>
    void info(Ts&&... ts)
    {
        write_endl(str_debug_, std::forward<Ts>(ts)...);
    }

    template <typename... Ts>
    void warning(Ts&&... ts)
    {
        write_endl(str_debug_, std::forward<Ts>(ts)...);
    }

    template <typename... Ts>
    void error(Ts&&... ts)
    {
        write_endl(str_debug_, std::forward<Ts>(ts)...);
    }

private:
    template <typename T>
    void write(T&& t)
    {
        if (file)
        {
            *file << t;
        }
        if (write_stdout)
        {
            std::cout << t;
        }
    }

    template <typename T, typename... Ts>
    void write(T&& t, Ts&&... ts)
    {
        write(std::forward<T>(t));
        write(std::forward<Ts>(ts)...);
    }

    template <typename... Ts>
    void write_endl(Ts&&... ts)
    {
        write(std::forward<Ts>(ts)...);
        // Doesn't work, not sure why, no google atm...
        // write(std::endl);
        if (write_stdout)
        {
            std::cout << std::endl;
        }
        if (file)
        {
            *file << std::endl;
        }
    }

private:
    std::string name_;
    std::string str_debug_;
    std::string str_info_;
    std::string str_warning_;
    std::string str_error_;
};

Logger& get_logger(std::string_view name);

namespace log
{
// Similar to Python - Default to root logger.
template <typename... Ts>
void debug(Ts&&... ts)
{
    get_logger("root").debug(std::forward<Ts>(ts)...);
}
template <typename... Ts>
void info(Ts&&... ts)
{
    get_logger("root").info(std::forward<Ts>(ts)...);
}
template <typename... Ts>
void warning(Ts&&... ts)
{
    get_logger("root").warning(std::forward<Ts>(ts)...);
}
template <typename... Ts>
void error(Ts&&... ts)
{
    get_logger("root").error(std::forward<Ts>(ts)...);
}
} // namespace log
