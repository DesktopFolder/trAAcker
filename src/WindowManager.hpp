#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <array>
#include <cstdint>
#include <string>

#include "logging.hpp"

// WindowManager.hpp
// For a reimplementation of AATool, we likely only require three distinct
// window contexts:
// 1. The overlay window. Overlays remaining objectives onto a monotone background.
// 2. The main window. Gives a full list of remaining objectives and advancements.
// 3. The settings window. For now, we will not bother with this. Later, use RmlUi.
//
// To abstract away management, we will create the WindowManager class and simply
// multiplex the two (or later, three) individual windows' events wherever we can.

namespace aa
{

enum class WindowID
{
    Main,
    Overlay,
};

enum class CloseMode
{
    Any,
    Main,
};

static constexpr uint8_t WINDOW_NUMER = 2;
static constexpr std::array<WindowID, WINDOW_NUMER> WINDOWS{WindowID::Main, WindowID::Overlay};

inline std::string wid_to_string(WindowID wid)
{
    switch (wid)
    {
    case WindowID::Main: return "main";
    case WindowID::Overlay: return "overlay";
    }
    throw std::runtime_error("Got bad window ID.");
}

class WindowManager
{
    WindowManager(/* TODO - Configuration from file */);

    sf::RenderWindow main_;
    sf::RenderWindow overlay_;
    CloseMode close_mode{CloseMode::Main};

    Logger& logger;

    std::array<sf::Color, WINDOW_NUMER> clearColours_;
    std::array<WindowID, WINDOW_NUMER> windows_{WindowID::Main, WindowID::Overlay};

public:
    static WindowManager& instance(/* TODO - Configuration from file */);

    bool handleEvent(sf::Event& event, WindowID id)
    {
        auto& window = getWindow(id);
        if (event.type == sf::Event::Closed)
        {
            logger.info("Got close event for wid: ", static_cast<int32_t>(id));
            window.close();
            return true;
        }
        else if (event.type == sf::Event::Resized)
        {
            logger.info("Got resize event for wid: ", static_cast<int32_t>(id));
            window.setView(sf::View(sf::FloatRect(0, 0, event.size.width, event.size.height)));
            return true;
        }
        return false;
    }

    std::vector<sf::Event> handleEvents()
    {
        std::vector<sf::Event> key_events;
        for (auto wid : std::array{WindowID::Main, WindowID::Overlay})
        {
            sf::Event event;
            auto& window = getWindow(wid);
            while (window.pollEvent(event))
            {
                if (handleEvent(event, wid)) continue;
                if (event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased)
                {
                    key_events.push_back(event); 
                    continue;
                }
                // Unhandled event.
            }
        }
        return key_events;
    }

    void clearAll()
    {
        for (auto wid : windows_)
        {
            auto& window = getWindow(wid);
            window.clear(clearColours_[static_cast<uint32_t>(wid)]);
        }
    }

    void displayAll()
    {
        for (auto wid : windows_)
        {
            auto& window = getWindow(wid);
            window.display();
        }
    }

    bool is_shutdown() const { 
        switch (close_mode) {
            case CloseMode::Main:
                return not main_.isOpen();
            case CloseMode::Any:
                return not main_.isOpen() or not overlay_.isOpen();
            default: std::terminate();
        }
    }

    auto& getMain() { return main_; }
    auto& getOverlay() { return overlay_; }
    sf::RenderWindow& getWindow(WindowID id)
    {
        switch (id)
        {
        case WindowID::Main: return getMain();
        case WindowID::Overlay: return getOverlay();
        }
        throw std::runtime_error("Got bad window ID.");
    }
};
} // namespace aa
