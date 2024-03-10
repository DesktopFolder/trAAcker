#include "Map.hpp"
#include "ConfigProvider.hpp"
#include "logging.hpp"

#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace aa
{
MapManager::MapManager(const nlohmann::json& config)
{
}

void MapManager::debug()
{
    auto& logger = get_logger("MapManager::debug");
}

bool consume_prefix(std::string_view& str, std::string_view prefix)
{
    if (not str.starts_with(prefix)) return false;
    str.remove_prefix(prefix.size());
    return true;
}

bool consume_n(std::string_view& str, uint64_t n)
{
    if (str.size() < n) return false;
    str.remove_prefix(n);
    return true;
}

std::optional<uint64_t> consume_coordinate(std::string_view& str)
{
    std::size_t pos{};
    uint64_t value{};
    try
    {
        value = std::stoll(std::string{str.data(), str.size()}, &pos);
    }
    catch(const std::exception& e)
    {
        return std::nullopt;
    }
    
    if (not consume_n(str, pos + 3)) return std::nullopt;
    return value;
}

std::optional<PlayerLocation> parseF3C(std::string_view f3c)
{
    // Let's ONLY parse f3+c for now. We could parse more if we wanted to later.
    // :)
    constexpr std::string_view overworld = "minecraft:overworld";
    constexpr std::string_view nether = "minecraft:the_nether";
    constexpr std::string_view end = "minecraft:the_end";
    constexpr std::string_view prefix = "/execute in ";
    constexpr std::string_view midfix = " run tp @s ";
    if (not consume_prefix(f3c, prefix)) return std::nullopt;
    PlayerLocation result{};
    if (consume_prefix(f3c, overworld))
    {
        result.dim = Dimension::Overworld;
    }
    else if (consume_prefix(f3c, nether))
    {
        result.dim = Dimension::Nether;
    }
    else if (consume_prefix(f3c, end))
    {
        result.dim = Dimension::End;
    }
    else { return std::nullopt; }
    if (not consume_prefix(f3c, midfix)) return std::nullopt;

    if (const auto x = consume_coordinate(f3c); x.has_value())
    {
        result.x = *x;
        if (const auto y = consume_coordinate(f3c); y.has_value())
        {
            result.y = *y;
            if (const auto z = consume_coordinate(f3c); z.has_value())
            {
                result.z = *z;
                return result;
            }
        }
    }
    return std::nullopt;
}

void MapManager::handleWorldChange(std::string_view old_world, std::string_view new_world)
{
    // Do nothing for now.
}

bool MapManager::updateFromClipboard(std::string_view clipboard)
{
    // `/execute in minecraft:overworld run tp @s 206.50 71.00 100.50 0.41 -0.41`
    auto& logger = get_logger("MapManager");
    if (const auto c = parseF3C(clipboard); c.has_value())
    {
        // wow! how cool. :) this is really awesome :)
        logger.debug("Got location: ", c->to_string());

        // Now we need to design a map. Basically. :)
        locations.push_back(*c);

        return true;
    }
    return false;
}
} // namespace aa