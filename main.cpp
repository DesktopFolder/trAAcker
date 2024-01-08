#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <filesystem>
#include <iostream>
#include <thread>

#include "app_finder.hpp"
#include "logging.hpp"

#include "src/ConfigProvider.hpp"
#include "src/FileProvider.hpp"
#include "src/Overlay.hpp"
#include "src/ResourceManager.hpp"
#include "src/WindowManager.hpp"

#include "src/Advancements.hpp"
#include "src/TurnTable.hpp"

int main()
{
    // Leave this at the start of main to ensure correct shutdown order.
    // Um... sort of dumb, but I believe in 'dumb but works until I fix it'
    // as opposed to 'leaving a gun on the table with the safety off'.
    // This is for logging - we preload the list of files object.
    // That way we close files at the very end.
    const auto& _ = detail::get_files();

    auto& conf = aa::conf::get();
    if (const auto logfile = aa::conf::get_if<std::string>(conf, "log"); logfile.has_value())
    {
        set_default_file(logfile.value());
    }
    const uint64_t sleep_ms = aa::conf::get_or(conf, "loop-sleep", 1);
    const bool vsync        = aa::conf::get_or(conf, "vsync", false);

    Logger::stdout_default = aa::conf::get_or(conf, "verbose", false);
    Logger::set_level(aa::conf::get_or<std::string>(conf, "log-level", "info"));
    // Any temporary test code goes here, we have logging now but haven't
    // really loaded anything up.

    // Todo: It would be really cool if we could do lazy/background loading
    // of assets here. Just launch off a std::thread and do window setup
    // simultaneously. Because this is just 'cool' I'm going to relegate
    // it for when I eventually (???) fix this up to be a proper application
    // and not just me throwing features into the main source file, lol.
    const auto manifest_source =
        aa::conf::get_or<std::string>(conf, "manifest", "advancements.json");
    auto manifest = AdvancementManifest::from_file(manifest_source);
    log::debug("Loaded manifest from ", manifest_source);

    // Doesn't do anything, we're just creating bindings.
    auto& window     = aa::WindowManager::instance().getOverlay();
    auto& mainwindow = aa::WindowManager::instance().getMain();
    window.setVerticalSyncEnabled(vsync);

    aa::OverlayManager ov(manifest, conf["overlay"]);

    aa::CurrentFileProvider fp;
    auto& rm = aa::ResourceManager::instance();
    auto& wm = aa::WindowManager::instance();

    uint64_t ticks = 0;
    log::debug("Starting main window loop.");
    while (!wm.is_shutdown())
    {
        // Handle events. We may need to add more stuff in here later.
        // Keep all this jammed in here until making an Application class later.
        // Or until making this all data driven/scripted? Not sure, really.
        auto events = wm.handleEvents();
        for (auto& event : events)
        {
            if (event.type == sf::Event::KeyReleased)
            {
                if (event.key.code == sf::Keyboard::A)
                {
                    log::debug("Got the key A!");
                }
                else if (event.key.code == sf::Keyboard::B)
                {
                    log::debug("Parsing test advancements file (1) - testing/all-everything.json");
                    ov.reset_from_file("testing/all-everything.json", manifest);
                }
                else if (event.key.code == sf::Keyboard::C)
                {
                    log::debug("Parsing test advancements file (2) - testing/no-recipes.json");
                    ov.reset_from_file("testing/no-recipes.json", manifest);
                }
                else if (event.key.code == sf::Keyboard::D)
                {
                    log::debug("Parsing test advancements file (3) - testing/less.json");
                    ov.reset_from_file("testing/less.json", manifest);
                }
                else if (event.key.code == sf::Keyboard::E)
                {
                    log::debug("Parsing test advancements file (4) - testing/most-complete.json");
                    ov.reset_from_file("testing/most-complete.json", manifest);
                }
                else if (event.key.code == sf::Keyboard::R)
                {
                    log::debug("Resetting to all advancements required.");
                    ov.reset(manifest);
                }
                else if (event.key.code == sf::Keyboard::P)
                {
                    log::debug("Dumping all available debug information.");
                    ov.debug();
                    log::debug("Ticks processed: ", ticks);
                    fp.debug();
                    log::debug("Finished dumping debug information.");
                }
            }
        }
        wm.clearAll();

        // poll after handling events...
        if (const auto result = fp.poll(ticks); result.has_value())
        {
            log::debug("Attempting to reset from found updated file: ", result.value());
            ov.reset_from_file(result.value(), manifest);
        }

        if (ticks % 60 == 0)
        {
            // call: providergetturntable?
            //auto s = get_focused_application(mainwindow);
            //log::debug("Current application maybe, unless this crashes! ", s.exec, s.name, s.pid);
            if (const auto fm = get_focused_minecraft(mainwindow); fm.has_value())
            {
                log::debug("Found a focused Minecraft instance path: ", *fm);
            }
        }

        /*
        for (int i = 0; i < window.getSize().x / 64; i++) {
            auto& tile = rb.get(i);
            s.setTexture(tile.text);
            s.setPosition(i * 64 + 8, 56);
            window.draw(s);
        }
        */
        ov.render(window);

        // Render loop end.
        wm.displayAll();

        // Give control to the OS - we don't want to consume too many resources.
        std::this_thread::sleep_for(std::chrono::milliseconds(sleep_ms));
        ticks += 1; // it's the completed # of ticks
    }
    if (mainwindow.isOpen())
    {
        mainwindow.close();
    }
    return 0;
}
