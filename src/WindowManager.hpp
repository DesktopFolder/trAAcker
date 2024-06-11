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
    Map,
    Reminders,
    Debug,
};

enum class CloseMode
{
    Any,
    Main,
};

static constexpr uint8_t NUMBER_OF_WINDOWS = 5;

inline std::string wid_to_string(WindowID wid)
{
    switch (wid)
    {
    case WindowID::Main: return "main";
    case WindowID::Overlay: return "overlay";
    case WindowID::Map: return "map";
    case WindowID::Reminders: return "reminders";
    case WindowID::Debug: return "debug";
    }
    throw std::runtime_error("Got bad window ID.");
}

inline std::string wid_to_prefix(WindowID wid)
{
    static const std::array<std::string, NUMBER_OF_WINDOWS> lut = []{
        std::array<std::string, NUMBER_OF_WINDOWS> ret;
        for (size_t i = 0; i < NUMBER_OF_WINDOWS; i++)
        {
            ret[i] = wid_to_string(static_cast<WindowID>(i)) + ": ";
        }
        return ret;
    }();
    return lut[static_cast<uint8_t>(wid)];
}

struct Window
{
    sf::RenderWindow window;
    WindowID id;

    sf::Color clearColour{0, 0, 0};

    void clear()
    {
        window.clear(clearColour);
    }

    enum Flags : uint8_t {
        MainWindow = 0b1, // Is this our main window?
    } flags;
};

class WindowManager
{
    WindowManager(/* TODO - Configuration from file */);

    std::array<Window, NUMBER_OF_WINDOWS> windows_;

    CloseMode close_mode{CloseMode::Main};

    Logger& logger;

public:
    static WindowManager& instance(/* TODO - Configuration from file */);

    bool handleEvent(sf::Event& event, WindowID id)
    {
        auto& window = get(id);
        if (event.type == sf::Event::Closed)
        {
            logger.info(wid_to_prefix(id), "got close event");
            window.close();
            return true;
        }
        else if (event.type == sf::Event::Resized)
        {
            logger.info(wid_to_prefix(id), "got resize event");
            window.setView(sf::View(sf::FloatRect(0, 0, event.size.width, event.size.height)));
            return true;
        }
        return false;
    }

    std::vector<sf::Event> handleEvents()
    {
        std::vector<sf::Event> key_events;
        for (auto wid : std::array{WindowID::Main, WindowID::Overlay, WindowID::Reminders, WindowID::Map, WindowID::Debug})
        {
            sf::Event event;
            auto& window = get(wid);
            if (not window.isOpen()) { continue; }
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
        for (auto& window : windows_)
        {
            if (window.id == aa::WindowID::Map) continue;
            window.clear();
        }
    }

    void displayAll()
    {
        for (auto& wid : windows_)
        {
            wid.window.display();
            // log::debug("displaying window: ", wid_to_string(wid));
        }
    }

    bool is_shutdown() const { 
        switch (close_mode) {
            case CloseMode::Main:
                return not get(WindowID::Main).isOpen();
            case CloseMode::Any:
                return not get(WindowID::Main).isOpen() or not get(WindowID::Overlay).isOpen();
            default: std::terminate();
        }
    }

    inline sf::RenderWindow& get(WindowID id)
    {
        auto iid = static_cast<uint8_t>(id);
        return windows_[iid].window;
    }
    inline const sf::RenderWindow& get(WindowID id) const
    {
        auto iid = static_cast<uint8_t>(id);
        return windows_[iid].window;
    }
};
} // namespace aa
