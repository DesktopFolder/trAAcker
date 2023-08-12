#pragma once

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

// WindowManager.hpp
// For a reimplementation of AATool, we likely only require three distinct
// window contexts:
// 1. The overlay window. Overlays remaining objectives onto a monotone background.
// 2. The main window. Gives a full list of remaining objectives and advancements.
// 3. The settings window. For now, we will not bother with this. Later, use RmlUi.
//
// To abstract away management, we will create the WindowManager class and simply
// multiplex the two (or later, three) individual windows' events wherever we can.

namespace aa {

class WindowManager {
    WindowManager(/* TODO - Configuration from file */);

    sf::RenderWindow main_;
    sf::RenderWindow overlay_;
public:
    static WindowManager& instance(/* TODO - Configuration from file */);

    bool is_shutdown() const { return not main_.isOpen(); }
    auto& getMain() { return main_; }
    auto& getOverlay() { return overlay_; }
};
}
