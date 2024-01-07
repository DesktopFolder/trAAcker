#pragma once

#include "ApplicationInfo.hpp"
#include "logging.hpp"

#include <optional>
#include <unordered_map>

namespace sf
{
class RenderWindow;
}

ApplicationInfo get_focused_application(sf::RenderWindow&);
std::optional<std::string> get_focused_minecraft(sf::RenderWindow& x);

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
// Windows specific code for getting the current application details.

#elif __APPLE__
// Apple specific code for getting the current application details.
#include "osx/FocusedApplicationFinder.hpp"

#include <cassert>
#include <sys/sysctl.h>

inline ApplicationInfo get_focused_application(sf::RenderWindow&)
{
    return osx::get_current_application();
}

inline std::optional<std::string> get_focused_minecraft(sf::RenderWindow& x)
{
    const auto& logger = get_logger("get_focused_minecraft");

    const auto s = get_focused_application(x);
    if (not s.exec.starts_with("java"))
    {
        logger.debug("Ignored application: ", s.exec);
        return {};
    }
    const int pid = s.pid;
    logger.debug("Got minecraft with PID: ", pid);

    // Note - something about this function is SUPER laggy, causing us
    // to skip around a little when we're running it 1x/60frames.
    // Cache the results because realistically speaking, that's fine.
    static std::unordered_map<int, std::optional<std::string>> cache;
    if (cache.contains(pid))
    {
        return cache[pid];
    }

    // https://gist.github.com/nonowarn/770696
    // WTF?

    int query_buf[3]{}; // Our queries live here.

    query_buf[0] = CTL_KERN;
    query_buf[1] = KERN_ARGMAX;

    int argmax     = 0;
    size_t argsize = sizeof(argmax);
    if (sysctl(query_buf, 2, &argmax, &argsize, NULL, 0) == -1)
    {
        logger.debug("Failed SYSCTL to acquire argument size: ", s.exec);
        return {};
    }

    // Allocate space for the arguments.
    std::vector<char> process_arguments;
    process_arguments.resize(argmax);

    // Set up our query buffer for the PROCARGS2 query.
    query_buf[0] = CTL_KERN;
    query_buf[1] = KERN_PROCARGS2;
    query_buf[2] = pid;

    argsize = (size_t)argmax;
    if (sysctl(query_buf, 3, process_arguments.data(), &argsize, NULL, 0) == -1)
    {
        logger.error("Failed SYSCTL to acquire process arguments: ", s.exec);
        return {};
    }

    // I THINK. THINK. That this is prefixed argc data.
    // Makes the most sense if you think that we are basically
    // injecting ourselves into "main(int argc, char** argv)",
    // and more or less just reading the memory present there.
    int argc = 0;
    memcpy(&argc, process_arguments.data(), sizeof(argc));

    size_t itr = sizeof(argc);

    /* Skip the saved exec_path. */
    assert(argsize <= process_arguments.size());
    for (; itr < argsize; itr++)
    {
        if (process_arguments[itr] == '\0')
        {
            break;
        }
    }

    /* Skip trailing '\0' characters. */
    for (; itr < argsize; itr++)
    {
        if (process_arguments[itr] != '\0')
        {
            /* Beginning of first argument reached. */
            break;
        }
    }

    if (itr == argsize)
    {
        logger.error("SYSCTL did not provide process arguments: ", s.exec);
        return {};
    }

    const size_t argument_start = itr;

    // DEBUG (get_focused_minecraft):
    // Got arguments: '/opt/homebrew/opt/openjdk/bin/java -Xdock:icon=icon.png
    // -Xdock:name="Prism Launcher: AA Tracker Testing" -XstartOnFirstThread
    // -Xms512m -Xmx4096m -Duser.language=en
    // -Djava.library.path=/Users/user/Library/Application Support/PrismLauncher/instances/AA Tracker Testing/natives
    // -cp /private/var/folders/l9/xlt__xp92pbg92834dhv38r40000gn/T/AppTranslocation/927FB467-2D15-47ED-8346-
    int arg_counter = 0;
    size_t start_location = argument_start;
    for (; arg_counter < argc && itr < argsize; itr++)
    {
        // itr >= start_location
        if (process_arguments[itr] == '\0')
        {
            // Increment argument counter.
            arg_counter++;

            // 'Understand' this argument.
            // argument is start_location -> itr
            const char* d = process_arguments.data();
            std::string_view sv(d + start_location);
            logger.debug("Found argument: ", sv, " of size: ", sv.size());
            std::string_view prefix = "-Djava.library.path=";
            if (sv.starts_with(prefix))
            {
                logger.debug("Found library path: discovering instance."); 
                std::string_view inst_str = "/instances/";
                size_t pos = sv.rfind(inst_str);
                if (pos == std::string_view::npos)
                {
                    logger.warning("Could not find instances path: no /instances/.");
                    cache.emplace(pid, std::nullopt);
                    return {};
                }
                
                size_t end = sv.find('/', pos + inst_str.size());

auto result = std::string{sv.substr(prefix.size(), end - prefix.size())};
cache.insert_or_assign(pid, result);
                return result;
            }

            // Start location is the NEXT character.
            start_location = itr + 1;
        }
    }
    
    /*
    // EXAMPLE CODE - SIMPLE
    int arg_counter = 0;
    size_t last_i   = 0;
    for (; arg_counter < argc && itr < argsize; itr++)
    {
        if (process_arguments[itr] == '\0')
        {
            arg_counter++;
            if (last_i != 0)
            {
                // Overwrite last \0 to ' '
                process_arguments[last_i] = ' ';
            }
            // Previous nullptr, overwrite to ' ' if we find another.
            last_i = itr;
        }
    }

     // sp points to the beginning of the arguments/environment string, and
     // np should point to the '\0' terminator for the string.
    if (last_i == 0)
    {
        // Empty or unterminated string.
        logger.error("Unterminated string? ", s.exec);
        return {};
    }

    std::string str = process_arguments.data() + start_location;
    logger.debug("Got arguments: '", str, "'")
        .debug("With argsize: ", argsize)
        .debug("And initial location: ", start_location)
        .debug("And final location: ", itr)
        .debug("And argc: ", argc);
    */

    cache.emplace(pid, std::nullopt);
    return {};
}

#elif __linux__
// Linux specific code for getting the current application details.
#else
#error "Unsupported OS? D:"
#endif
