#include "Overlay.hpp"
#include "logging.hpp"

#include <fstream>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

std::string getStrOr(auto& j, const std::string& key, const std::string& def)
{
    if (j.contains(key)) return j[key].template get<std::string>();
    return def;
}

std::string parseID(auto& j)
{
    std::string id = j["@id"].template get<std::string>();
    auto it        = std::find(id.crbegin(), id.crend(), '/');
    if (it == id.crend()) return id;
    return std::string{it.base(), id.cend()};
}

std::string parseIcon(auto& j)
{
    if (j.contains("@icon")) return j["@icon"].template get<std::string>();
    // return parseID(j);
    return "";
}

aa::OverlayManager::OverlayManager()
{
    const auto& logger = get_logger("OverlayManager");
    prereqs.yOffset_   = reqs.PADDING;
    reqs.yOffset_      = reqs.TILE_SIZE;

    // testing code
    testText.loadFromFile("assets/sprites/global/items/conduit^48.png");
    prereqs.rb_.buf().emplace_back("conduit", &testText);
    reqs.rb_.buf().emplace_back("conduit", &testText);

    std::ifstream f("advancements.json");
    if (!f.good()) throw std::runtime_error("No advancements.json found");
    auto advancements = json::parse(f);

    auto& rm    = aa::ResourceManager::instance();

    logger.debug("Loading map of available assets.");
    auto assets = aa::ResourceManager::getAllAssets();
    for (auto& [k, v] : assets)
    {
        logger.debug("Asset named ", k, " located at ", v);
    }
    logger.debug("Was able to find ", assets.size(), " potential assets.");

    for (auto& k : advancements)
    {
        logger.debug("Found advancement category: ", k["@name"],
                     ". Now iterating advancements...");
        const std::string cat = k["@name"].get<std::string>();
        for (auto& a : k["advancement"])
        {
            this->advancements.emplace_back(cat, parseID(a), parseIcon(a));
            auto& adv = this->advancements.back();
            if (adv.icon != "")
            {
                if (not assets.contains(adv.icon))
                    throw std::runtime_error("Could not find icon " + adv.icon);
                reqs.rb_.buf().emplace_back(adv.name, rm.store_texture_at(assets[adv.icon]));
                logger.debug("Loaded explicit icon for ", adv.name,
                             " from file: ", assets[adv.icon]);
                continue;
            }
            // try and load it based on the name
            if (adv.name == "")
            {
                logger.warning("Found an advancement with no name, category: ", adv.category);
                continue;
            }
            if (assets.contains(adv.name)) {
                reqs.rb_.buf().emplace_back(adv.name, rm.store_texture_at(assets[adv.name]));
                logger.debug("Loaded implicit icon for ", adv.name, " from file: ", assets[adv.name]);
                continue;
            }
            logger.error("Could not load asset for name: ", adv.name);
        }
    }

    logger.info("Properly loaded ", reqs.rb_.buf().size(), " tiles.");
}

void aa::OverlayManager::reset()
{
    auto& rm = aa::ResourceManager::instance();

    prereqs.reset();
    reqs.reset();

    for (const auto& adv : advancements)
    {
        // what is key, what is value?
        // key is definitely advancement name, value is lookup..?
        // well, the graphics are all present in the resource manager
    }
}
