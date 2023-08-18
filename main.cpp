#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <filesystem>
#include <iostream>
#include <thread>

#include "src/ConfigProvider.hpp"
#include "src/WindowManager.hpp"
#include "src/ResourceManager.hpp"
#include "src/Overlay.hpp"
#include "src/FileProvider.hpp"

#include "src/TurnTable.hpp"

// for now, just do everything in this one file
// to get used to marks. and not out of laziness. yeah.

int main()
{
    auto& window     = aa::WindowManager::instance().getOverlay();
    auto& mainwindow = aa::WindowManager::instance().getMain();
    window.setVerticalSyncEnabled(true);
    auto text = sf::Texture{};
    text.loadFromFile("assets/sprites/global/items/conduit^48.png");
    sf::Sprite s(text);
    //sf::View view;
    
    //aa::RingBuffer<aa::Tile> rb;
    // aa::TurnTable tb;
    //tb.rb_.buf().emplace_back("conduit", text);

    aa::OverlayManager ov;

    // Initialize the view to a rectangle located at (100, 100) and with a size of 400x200
    //view.reset(sf::FloatRect(0, 0, 400, 400));

    // where we render on the destination.
    // view.setViewport(sf::FloatRect(0.f, 0.f, 0.5f, 1.f));

    auto& conf = aa::conf::get();
    // should really be overlay.rate but whatever
    uint8_t rate = 0;
    if (conf.contains("rate")) {
        ov.setRate(static_cast<uint8_t>(conf["rate"].template get<int>()));
    }
    uint64_t ticks = 0;

    aa::CurrentFileProvider fp;
    auto& rm = aa::ResourceManager::instance();
    auto& wm = aa::WindowManager::instance();
    while (!wm.is_shutdown())
    {
        // Handle events. We may need to add more stuff in here later.
        wm.handleEvents();
        wm.clearAll();

        // poll after handling events...
        fp.poll();

        if (ticks % 60 == 0) {
            // call: providergetturntable?
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
        std::this_thread::sleep_for(std::chrono::seconds(0));
        ticks += 1; // it's the completed # of ticks
    }
    if (mainwindow.isOpen())
    {
        mainwindow.close();
    }
    return 0;
}
