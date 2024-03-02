#pragma once

#include "utilities.hpp"
#include "logging.hpp"
#include <SFML/Window/Clipboard.hpp>
#include <SFML/Window/Keyboard.hpp>

#include <nlohmann/json_fwd.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

namespace aa
{
struct MapManager
{
    MapManager(const nlohmann::json& config);

    void debug();

    void render(sf::RenderWindow& win, uint64_t ticks)
    {
        // still assume 60fps, recommended 5hz, so 5x/s
        // so, 5 in 60 frames, so 1/15
        if (ticks % 13 == 0)
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::C) && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::C))
            {
                updateQueued = true;
            }
        }
        
        if (ticks % 60 == 0 && updateQueued)
        {
            get_logger("MapManager").debug(std::string{sf::Clipboard::getString()});
        }
    }

    bool updateQueued {false};
};
}  // namespace aa
