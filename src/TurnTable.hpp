#pragma once

#include "ResourceManager.hpp"
#include "RingBuffer.hpp"
#include "Tile.hpp"
#include "logging.hpp"
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>

namespace aa
{
struct TurnTable
{
    void animateDraw(sf::RenderWindow& win)
    {
        auto& logger = get_logger("TurnTable::animateDraw");

        if (rb_.size() == 0)
        {
            // logger.debug("Empty buffer - did not render.");
            return;
        }

        // This is not stellar...
        sf::Sprite sprite;

        const auto winX = win.getSize().x;

        const auto TO_DRAW = (winX / TurnTable::tile_size) + 2;

        uint64_t first = rb_.get(0).text->getSize().x;
        for (int64_t i = 0; i < TO_DRAW; i++)
        {
            // set the texture of the sprite
            const auto& [name, texture, draw_bg] = rb_.get(i);
            const auto xv                        = texture->getSize().x;

            if (first != xv)
            {
                // Basically an assertion.
                logger.fatal_error("Got first: ", first, " that is not equal to xv: ", xv);
            }

            sprite.setTexture(*texture);
            // Integer math. Consistent & fine.
            const auto generic_offset = static_cast<float>(tile_size * i - offset_ + xOffset_);
            const auto generic_centroid = generic_offset + (xv / 2);
            sprite.setPosition(generic_offset, yOffset_);
            win.draw(sprite);

            if (drawText)
            {
                // Temporary! I'm sure lol :)
                if (name.find(' ') != std::string::npos)
                {
                    sf::Text t1, t2;
                    auto [s1, s2] = basic_string_splitter(name);
                    t1.setFont(aa::ResourceManager::instance().get_font());
                    t1.setString(s1);
                    t1.setCharacterSize(fontSize);
                    t1.setFillColor(sf::Color::White);

                    auto fr = t1.getLocalBounds();
                    t1.setPosition(generic_centroid - (fr.left + fr.width / 2), yOffset_ + inner_size + padding);
                    win.draw(t1);

                    t2.setFont(aa::ResourceManager::instance().get_font());
                    t2.setString(s2);
                    t2.setCharacterSize(fontSize);
                    t2.setFillColor(sf::Color::White);

                    fr = t2.getLocalBounds();
                    t2.setPosition(generic_centroid - (fr.left + fr.width / 2), yOffset_ + inner_size + padding + fontSize + 1);
                    win.draw(t2);

                    // THIS IS SO HANDY. I LOVE IT.
                    // win.draw(aa::ResourceManager::basic_marker(2, generic_centroid, yOffset_ + inner_size + padding));
                }
                else
                {
                    sf::Text t;
                    t.setFont(aa::ResourceManager::instance().get_font());
                    t.setString(name);
                    t.setCharacterSize(fontSize);
                    t.setFillColor(sf::Color::White);

                    auto fr = t.getLocalBounds();
                    t.setPosition(generic_centroid - (fr.left + fr.width / 2), yOffset_ + inner_size + padding);
                    win.draw(t);
                }
            }
        }

        // now we animate, because we're bad, and this is easier, and then we start from 0
        offset_ += rate_;
        if (offset_ >= tile_size)
        {
            rb_.shift(); // We consumed a full tile. Shift over once.
            offset_ = 0;
        }
    }

    void reset() { rb_.pos_ = 0; }
    void clear() { rb_.buf_.clear(); }

    template <typename... Ts>
    void emplace(Ts&&... ts)
    {
        rb_.buf_.emplace_back(std::forward<Ts>(ts)...);
    }

    void set_padding(int64_t value)
    {
        padding = value;
        update_validate();
    }

    void set_size(int64_t value)
    {
        inner_size = value;
        update_validate();
    }

    int64_t get_padding() { return padding; }

    int64_t get_size() { return tile_size; }

    int64_t get_texture_size() { return inner_size; }

    void update_validate() { tile_size = padding * 2 + inner_size; }

    // Storage type - ring buffer of tiles
    RingBuffer<Tile> rb_;
    sf::Texture background_;
    // How far we are, into the current tile
    int64_t offset_ = 0;

    // Configuration
    uint8_t rate_    = 1;
    float yOffset_   = 0;
    int64_t xOffset_ = 8;

    bool drawText = false;
    uint8_t fontSize = 12;

private:
    // Keep these private. Change with set_*
    int64_t padding    = 4;
    int64_t inner_size = 48;
    // Automatically updated through update_validate.
    int64_t tile_size = padding * 2 + inner_size;
};
} // namespace aa
