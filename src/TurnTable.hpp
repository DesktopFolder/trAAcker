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

        sf::Sprite sprite16;
        sf::Sprite sprite32;
        sf::Sprite sprite512;
        sf::Sprite sprite48;
        sprite16.setScale(3, 3);
        sprite32.setScale(1.5, 1.5);
        sprite512.setScale(0.09375, 0.09375);

        const auto winX = win.getSize().x;

        const auto TO_DRAW = (winX / TurnTable::TILE_SIZE) + 2;

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
                // wtf?
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
            sprite->setPosition(static_cast<float>((TILE_SIZE * i) - offset_) + 8.f, yOffset_);
            win.draw(*sprite);
        }

        // now we animate, because we're bad, and this is easier, and then we start from 0
        offset_ += rate_;
        if (offset_ >= TILE_SIZE)
        {
            rb_.shift(); // We consumed a full tile. Shift over once.
            offset_ = 0;
        }
    }

    void reset() { rb_.pos_ = 0; }

    // Storage type - ring buffer of tiles
    RingBuffer<Tile> rb_;
    // How far we are, into the current tile
    int64_t offset_ = 0;

    // Configuration
    uint8_t rate_  = 1;
    float yOffset_ = 0;
    // Constant for now
    constexpr static int64_t PADDING    = 4;
    constexpr static int64_t INNER_SIZE = 48;
    constexpr static int64_t TILE_SIZE  = PADDING * 2 + INNER_SIZE;
};
} // namespace aa
