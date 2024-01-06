#include "WindowManager.hpp"

#include <array>

#include "ConfigProvider.hpp"

aa::WindowManager& aa::WindowManager::instance()
{
    static aa::WindowManager mgr{};
    return mgr;
}

aa::WindowManager::WindowManager()
    : main_(sf::VideoMode(1280, 720), "trAAcker"),
      overlay_(sf::VideoMode(800, 200), "trAAcker OBS Overlay"),
      logger(get_logger("WindowManager")), clearColours_{}
{
    for (auto& col : clearColours_)
    {
        col = sf::Color(0, 0, 0);
    }

    // Configuration
    auto conf = aa::conf::getNS("window");
    if (conf.empty()) return;

    if (aa::conf::get_or(conf, "close-on", std::string{}) == "any")
    {
        close_mode = CloseMode::Any;
    }

    for (auto wid : WINDOWS)
    {
        auto wstr = wid_to_string(wid);
        if (conf.contains(wstr))
        {
            auto& wconf = conf[wstr];
            if (wconf.contains("bg"))
            {
                // Wow, we can set a config value
                auto cl   = wconf["bg"].template get<std::array<uint8_t, 3>>();
                auto& col = clearColours_[static_cast<uint32_t>(wid)];
                col.r     = cl[0];
                col.g     = cl[1];
                col.b     = cl[2];
            }

            aa::conf::apply(wconf, "title",
                            [&](std::string title) { getWindow(wid).setTitle(title); });
        }
    }
}
