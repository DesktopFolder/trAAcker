#pragma once

#include <string>

struct ApplicationInfo {
    std::string name;
    std::string exec;
    int pid = 0; 
};
