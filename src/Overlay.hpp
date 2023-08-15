#pragma once

// What is the overlay?
// It's basically a set of turntables.
// Oh, how the turn tables.
#include "TurnTable.hpp"

namespace aa
{
struct OverlayManager
{
    TurnTable prereqs;
    TurnTable reqs;
    sf::Texture testText;

    OverlayManager();

    void setRate(uint8_t rate)
    {
        prereqs.rate_ = rate;
        reqs.rate_    = rate;
    }

    void render(sf::RenderWindow& win)
    {
        prereqs.animateDraw(win);
        reqs.animateDraw(win);
    }
};
}  // namespace aa
