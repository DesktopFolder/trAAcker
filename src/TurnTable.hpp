#pragma once

#include "RingBuffer.hpp"
#include "Tile.hpp"
#include <SFML/Graphics/RenderWindow.hpp>

namespace aa
{
struct TurnTable
{
    void animateDraw(sf::RenderWindow& win) {
        if (rb_.size() == 0) return;

        sf::Sprite sprite;

        const auto winX = win.getSize().x;

        const auto TO_DRAW = (winX / TurnTable::TILE_SIZE) + 2;

        for (int64_t i = 0; i < TO_DRAW; i++) {
            sprite.setPosition(static_cast<float>((TILE_SIZE * i) - offset_) + 8.f, yOffset_);
            // set the texture of the sprite
            const auto& [name, texture, draw_bg] = rb_.get(i);
            sprite.setTexture(texture);
            win.draw(sprite);
        }
        
        // now we animate, because we're bad, and this is easier, and then we start from 0
        offset_ += rate_;
        if (offset_ >= TILE_SIZE) {
            rb_.shift(); // We consumed a full tile. Shift over once.
            offset_ = 0;
        }
    }

    void reset() {
        rb_.pos_ = 0;
    }

    // Storage type - ring buffer of tiles
    RingBuffer<Tile> rb_;
    // How far we are, into the current tile
    int64_t offset_ = 0;

    // Configuration
    uint8_t rate_ = 1;
    float yOffset_ = 0;
    // Constant for now
    constexpr static int64_t PADDING = 4;
    constexpr static int64_t INNER_SIZE = 48;
    constexpr static int64_t TILE_SIZE = PADDING * 2 + INNER_SIZE;
};
}  // namespace aa
