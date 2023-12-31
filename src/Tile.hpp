#pragma once

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace aa
{
struct Tile
{
    std::string name;
    const sf::Texture* text;
    bool render_bg = false;

    // Can't remove this if we want to support like, 95% of compilers.
    // Sad face.
    Tile(std::string name_, const sf::Texture* text_, bool render_bg_ = false)
        : name(std::move(name_)), text(text_), render_bg(render_bg_)
    {
    }
};
} // namespace aa
