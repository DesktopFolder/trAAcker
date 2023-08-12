#include <iostream>
#include <filesystem>

#include "include/dmon.hpp"

// for now, just do everything in this one file
// to get used to marks. and not out of laziness. yeah.

int main()
{
    auto w = dmon::Manager::instance().add_watch("./testdir");
    while (!w.triggered_) {

    }
    return 0;
}
