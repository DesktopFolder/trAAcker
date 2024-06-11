#pragma once

#include <cstdint>
#include <string>

namespace aa
{
struct AppConfig
{
    const uint64_t sleep_ms;
    const bool vsync;

    const std::string manifest;
};

struct Application {
    static AppConfig configure();

    void run();

    AppConfig conf = Application::configure();
};
}