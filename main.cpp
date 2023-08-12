#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <filesystem>
#include <iostream>

#include "include/dmon.hpp"
#include "src/WindowManager.hpp"

// for now, just do everything in this one file
// to get used to marks. and not out of laziness. yeah.

int main() {
    /*
    auto w = dmon::Manager::instance().add_watch("./testdir");
    while (!w.triggered_) {

    }
    */
    auto& window = aa::WindowManager::instance().getOverlay();
    auto& mainwindow = aa::WindowManager::instance().getMain();
    window.setVerticalSyncEnabled(true);
    auto text = sf::Texture{};
    text.loadFromFile("assets/sprites/global/items/conduit^32.png");
    sf::Sprite s(text);
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
        }
        if (mainwindow.isOpen()) {
            while (mainwindow.pollEvent(event)) {
                if (event.type == sf::Event::Closed) mainwindow.close();
            }
        }

        window.clear(sf::Color(176, 187, 192));
        window.draw(s);
        window.display();
        
        usleep(0);
    }
    if (mainwindow.isOpen()) {
        mainwindow.close();
    }
    return 0;
}
