#pragma once

// What is the overlay?
// It's basically a set of turntables.
// Oh, how the turn tables.
#include "TurnTable.hpp"

namespace aa {
struct Overlay {
    TurnTable prereqs;
    TurnTable reqs;
    sf::Texture testText;

    Overlay() {
        prereqs.yOffset_ = reqs.PADDING;
        reqs.yOffset_ = reqs.TILE_SIZE;

        // testing code
        testText.loadFromFile("assets/sprites/global/items/conduit^48.png");
    }

    void render(sf::RenderWindow& win) {
        prereqs.animateDraw(win);
        reqs.animateDraw(win);
    }
};
}
