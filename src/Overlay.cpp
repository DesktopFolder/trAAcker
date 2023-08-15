#include "Overlay.hpp"

#include <fstream>
#include <nlohmann/json.hpp>
#include <iostream>
using json = nlohmann::json;

aa::OverlayManager::OverlayManager()
{
    prereqs.yOffset_ = reqs.PADDING;
    reqs.yOffset_    = reqs.TILE_SIZE;

    // testing code
    testText.loadFromFile("assets/sprites/global/items/conduit^48.png");
    prereqs.rb_.buf().emplace_back("conduit", testText);
    reqs.rb_.buf().emplace_back("conduit", testText);

    std::ifstream f("advancements.json");
    if (!f.good()) throw std::runtime_error("No advancements.json found");
    auto advancements = json::parse(f);

    for (auto& k : advancements) {
        std::cout << k["@name"] << std::endl;
    }
}
