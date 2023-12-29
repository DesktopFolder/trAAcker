#pragma once

#include <string>
#include <vector>
#include "utilities.hpp"

struct Advancement
{
    std::string name;
    std::vector<std::string> completed_criteria;
    bool completed{false};
};

string_map<Advancement> get_advancements(std::string_view filename);
