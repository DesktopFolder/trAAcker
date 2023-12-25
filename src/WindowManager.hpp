#pragma once

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <array>

#include <iostream>
#include <cstdint>
#include <string>

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

enum WindowID {
    Main,
    Overlay,
};

static constexpr uint8_t WINDOW_NUMER = 2;
static constexpr std::array<WindowID, WINDOW_NUMER> WINDOWS{WindowID::Main, WindowID::Overlay};

inline std::string wid_to_string(WindowID wid) {
    switch (wid) {
        case WindowID::Main: return "main";
        case WindowID::Overlay: return "overlay";
    }
    throw std::runtime_error("Got bad window ID.");
}

class WindowManager {
    WindowManager(/* TODO - Configuration from file */);

    sf::RenderWindow main_;
    sf::RenderWindow overlay_;

    std::array<sf::Color, WINDOW_NUMER> clearColours_;
    std::array<WindowID, WINDOW_NUMER> windows_{WindowID::Main, WindowID::Overlay};
public:
    static WindowManager& instance(/* TODO - Configuration from file */);

    bool handleEvent(sf::Event& event, WindowID id) {
        auto& window = getWindow(id);
        if (event.type == sf::Event::Closed) {
            // std::cout << "Got close event for wid: " << static_cast<int32_t>(id) << std::endl;
            window.close();
            return true;
        }
        else if (event.type == sf::Event::Resized) {
            // std::cout << "Got resize event for wid: " << static_cast<int32_t>(id) << std::endl;
            window.setView(sf::View(sf::FloatRect(0, 0, event.size.width, event.size.height)));
            return true;
        }
        return false;
    }

    void handleEvents() {
        for (auto wid : std::array{WindowID::Main, WindowID::Overlay}) {
            sf::Event event;
            auto& window = getWindow(wid);
            while (window.pollEvent(event)) {
                if (handleEvent(event, wid)) continue;
                // Unhandled event.
            }
        }
    }

    void clearAll() {
        for (auto wid : windows_) {
            auto& window = getWindow(wid);
            window.clear(clearColours_[wid]);
        }
    }

    void displayAll() {
        for (auto wid : windows_) {
            auto& window = getWindow(wid);
            window.display();
        }
    }

    bool is_shutdown() const { return not main_.isOpen(); }
    auto& getMain() { return main_; }
    auto& getOverlay() { return overlay_; }
    sf::RenderWindow& getWindow(WindowID id) {
        switch (id) {
            case WindowID::Main: return getMain();
            case WindowID::Overlay: return getOverlay();
        }
        throw std::runtime_error("Got bad window ID.");
    }
};
}
