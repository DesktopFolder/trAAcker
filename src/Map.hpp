#pragma once

#include "utilities.hpp"
#include "logging.hpp"
#include <SFML/Window/Clipboard.hpp>
#include <SFML/Window/Keyboard.hpp>

#include <nlohmann/json_fwd.hpp>
#include <fmt/core.h>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

namespace aa
{
enum class Dimension
{
    Overworld,
    Nether,
    End,
};

inline std::string to_string(Dimension dimension)
{
    switch(dimension)
    {
        case Dimension::Overworld:
            return "Overworld";
        case Dimension::Nether:
            return "Nether";
        case Dimension::End:
            return "End";
        default:
            throw std::runtime_error{"Bad dimension value."};
    }
}

enum class LocationTag
{
    Untagged,
    Spawn,
    Special
};

// They can't spell colour idk it's weird
inline sf::Color to_colour(LocationTag tag)
{
    switch(tag)
    {
        case LocationTag::Untagged:
            return sf::Color::Cyan;
        case LocationTag::Spawn:
            return sf::Color::Magenta;
        case LocationTag::Special:
            return sf::Color::Red;
        default:
            return sf::Color::Blue;
    }
}

struct PlayerLocation
{
    Dimension dim;
    int64_t x;
    int64_t y;
    int64_t z;
    LocationTag tag;

    std::string to_string() const
    {
        return fmt::format("PlayerLocation({}: {}, {}, {})", ::aa::to_string(dim), x, y, z);
    }
};

struct MapManager
{
    /*
    https://en.sfml-dev.org/forums/index.php?topic=27998.0
    https://en.sfml-dev.org/forums/index.php?topic=11291.0
    https://github.com/SFML/SFML/wiki/Source%3A-Zoom-View-At-%28specified-pixel%29
    https://en.sfml-dev.org/forums/index.php?topic=18760.0
    https://en.sfml-dev.org/forums/index.php?topic=27467.0
    https://www.sfml-dev.org/tutorials/2.6/graphics-shape.php
    */
    MapManager(const nlohmann::json& config);

    void debug();

    void render(sf::RenderWindow& win, uint64_t ticks)
    {
        // still assume 60fps, recommended 5hz, so 5x/s
        // so, 5 in 60 frames, so 1/12
        // lol let's be more accurate :) - PAAcMAAnMVC
        if (ticks % 4 == 0)
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::C) && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::F3))
            {
                updateQueued = true;
            }
        }
        
        // If we have an update, do our update, y'know? Y'get what I'm sayin'?
        if (ticks % 60 == 0 && updateQueued)
        {
            const auto cb = std::string{sf::Clipboard::getString()};
            get_logger("MapManager").debug("Got clipboard: ", cb);
            updateQueued = false;
            if (not updateFromClipboard(cb)) return;
        }
        else { return; }

        if (locations.empty()) return;

        // Okay :)
        // Let's draw!!! BINGO BANGO!
        const auto& loc = locations.back();
        // TODO - FULL RERENDER HERE. OBVIOUSLY.
        if (loc.dim != current_dimension) { current_dimension = loc.dim; }
        drawAll(win);
    }

    void drawAll(sf::RenderWindow& win)
    {
        win.clear(sf::Color::Black);
        for (const auto& loc : locations)
        {
            if (loc.dim != current_dimension) continue;
            drawLocation(win, loc);
        }
    }

    void drawLocation(sf::RenderWindow& win, const PlayerLocation& loc)
    {
        const float x = loc.x / zoomLevel;
        const float z = loc.z / zoomLevel;
        const auto sz = win.getSize();
        sf::RectangleShape point(sf::Vector2f(3.f, 3.f));
        point.setPosition({x, z});
        // log::debug(x, " ", z);
        point.setFillColor(to_colour(loc.tag));
        win.draw(point);
    }

    int64_t zoomLevel = 10;
    Dimension current_dimension = Dimension::Overworld;
    std::vector<PlayerLocation> locations;

    void handleWorldChange(std::string_view old_world, std::string_view new_world);

    bool updateFromClipboard(std::string_view clipboard);

    bool updateQueued {false};
};
}  // namespace aa
