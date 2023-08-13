#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <filesystem>
#include <iostream>

#include "include/dmon.hpp"
#include "src/WindowManager.hpp"

#include "src/RingBuffer.hpp"
#include "src/Tile.hpp"

// for now, just do everything in this one file
// to get used to marks. and not out of laziness. yeah.

int main()
{
    /*
    auto w = dmon::Manager::instance().add_watch("./testdir");
    while (!w.triggered_) {

    }
    */
    auto& window     = aa::WindowManager::instance().getOverlay();
    auto& mainwindow = aa::WindowManager::instance().getMain();
    window.setVerticalSyncEnabled(true);
    auto text = sf::Texture{};
    text.loadFromFile("assets/sprites/global/items/conduit^48.png");
    sf::Sprite s(text);
    //sf::View view;
    
    aa::RingBuffer<aa::Tile> rb;
    rb.buf().emplace_back("conduit", text);

    // Initialize the view to a rectangle located at (100, 100) and with a size of 400x200
    //view.reset(sf::FloatRect(0, 0, 400, 400));

    // where we render on the destination.
    // view.setViewport(sf::FloatRect(0.f, 0.f, 0.5f, 1.f));

    auto& wm = aa::WindowManager::instance();
    while (!wm.is_shutdown())
    {
        // Handle events. We may need to add more stuff in here later.
        wm.handleEvents();
        wm.clearAll();
        window.draw(s);

        for (int i = 0; i < window.getSize().x / 64; i++) {
            auto& tile = rb.get(i);
            s.setTexture(tile.text);
            s.setPosition(i * 64 + 8, 56);
            window.draw(s);
        }

        // Render loop end.
        wm.displayAll();

        // Give control to the OS - we don't want to consume too many resources.
        usleep(0);
    }
    if (mainwindow.isOpen())
    {
        mainwindow.close();
    }
    return 0;
}
