#pragma once

#include "RingBuffer.hpp"
#include "Tile.hpp"
#include "logging.hpp"
#include <SFML/Graphics/RenderWindow.hpp>

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
        sf::Sprite sprite16;
        sf::Sprite sprite32;
        sf::Sprite sprite48;
        sf::Sprite sprite512;

        float sz = inner_size;
        sprite16.setScale(sz / 16.0, sz / 16.0);
        sprite32.setScale(sz / 32.0, sz / 32.0);
        sprite48.setScale(sz / 48.0, sz / 48.0);
        sprite512.setScale(sz / 512.0, sz / 512.0);

        const auto winX = win.getSize().x;

        const auto TO_DRAW = (winX / TurnTable::tile_size) + 2;

        for (int64_t i = 0; i < TO_DRAW; i++)
        {
            sf::Sprite* sprite;
            // set the texture of the sprite
            const auto& [name, texture, draw_bg] = rb_.get(i);
            const auto xv                        = texture->getSize().x;
            if (xv == 16)
            {
                sprite = &sprite16;
            }
            else if (xv == 32)
            {
                sprite = &sprite32;
            }
            else if (xv == 512)
            {
                // these are (well, it's just one, but) animated sprites
                // really need to clip our actual sprite out
                sprite = &sprite512;
            }
            else
            {
                if (xv != 48)
                {
                    logger.fatal_error("Got texture size that is invalid? ", xv,
                                       " for texture: ", name);
                }
                sprite = &sprite48;
            }

            sprite->setTexture(*texture);
            sprite->setPosition(static_cast<float>((tile_size * i) - offset_) + 8.f, yOffset_);
            win.draw(*sprite);
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

    template<typename... Ts>
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

    int64_t get_padding()
    {
        return padding;
    }

    int64_t get_size()
    {
        return tile_size;
    }

    void update_validate()
    {
        tile_size = padding * 2 + inner_size;
    }

    // Storage type - ring buffer of tiles
    RingBuffer<Tile> rb_;
    sf::Texture background_;
    // How far we are, into the current tile
    int64_t offset_ = 0;

    // Configuration
    uint8_t rate_  = 1;
    float yOffset_ = 0;

private:
    // Keep these private. Change with set_*
    int64_t padding    = 4;
    int64_t inner_size = 48;
    // Automatically updated through update_validate.
    int64_t tile_size  = padding * 2 + inner_size;
};
} // namespace aa
