#include "WindowManager.hpp"

#include <array>

#include "ConfigProvider.hpp"

namespace aa
{
WindowManager& WindowManager::instance()
{
    static WindowManager mgr{};
    return mgr;
}

Window factory(WindowID id)
{
    switch(id)
    {
        case WindowID::Main:
        return {{sf::VideoMode(1280, 720), "trAAcker"}, id};
        case WindowID::Overlay:
        return {{sf::VideoMode(800, 200), "trAAcker OBS Overlay"}, id};
        case WindowID::Map:
        return {{sf::VideoMode(400, 400), "trAAcker Map"}, id};
        case WindowID::Reminders:
        return {{sf::VideoMode(400, 400), "trAAcker Reminders"}, id};
        case WindowID::Debug:
        return {{sf::VideoMode(400, 400), "trAAcker Debug"}, id};
    }
}

WindowManager::WindowManager()
    : windows_{factory(WindowID::Main), factory(WindowID::Overlay), factory(WindowID::Map), factory(WindowID::Reminders), factory(WindowID::Debug)},
      logger(get_logger("WindowManager"))
{
    // Configuration
    auto conf = aa::conf::getNS("window");
    if (conf.empty()) return;

    if (aa::conf::get_or(conf, "close-on", std::string{}) == "any")
    {
        close_mode = CloseMode::Any;
    }

    for (auto& window : windows_)
    {
        auto wstr = wid_to_string(window.id);
        if (conf.contains(wstr))
        {
            auto& wconf = conf[wstr];
            if (wconf.contains("bg"))
            {
                // Wow, we can set a config value
                auto cl   = wconf["bg"].template get<std::array<uint8_t, 3>>();
                auto& col = window.clearColour;
                col.r     = cl[0];
                col.g     = cl[1];
                col.b     = cl[2];
            }

            aa::conf::apply(wconf, "title",
                            [&](std::string title) { window.window.setTitle(title); });
        }
    }
}
} // namespace aa