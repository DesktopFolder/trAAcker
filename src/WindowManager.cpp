#include "WindowManager.hpp"

aa::WindowManager& aa::WindowManager::instance() {
    static aa::WindowManager mgr{};
    return mgr;
}

aa::WindowManager::WindowManager() : main_(sf::VideoMode(1280, 720), "AATool--"), overlay_(sf::VideoMode(800, 200), "AATool-- Overlay") { 
    
}
