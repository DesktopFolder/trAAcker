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

auto parseID(auto& j)
{
    struct ParseResult
    {
        std::string full_id;
        std::string icon_id;
    };
    std::string id                   = j["@id"].template get<std::string>();
    const std::string_view mc_prefix = "minecraft:";
    if (id.starts_with(mc_prefix))
    {
        id = id.substr(mc_prefix.size());
    }

    // now we have a normalized id, something like: end/root or hello/world
    auto it = std::find(id.crbegin(), id.crend(), '/');
    // this shouldn't happen, but in case it does, handle it normally
    if (it == id.crend()) return ParseResult{id, id};
    // otherwise, return full id AND icon id
    return ParseResult{id, std::string{it.base(), id.cend()}};
}

std::string parseIcon(auto& j)
{
    if (j.contains("@icon")) return j["@icon"].template get<std::string>();
    // return parseID(j);
    return "";
}

// Current opinion on this code - not very good
// Seems ridiculously stateful and over the top for performance when it's not really...
// like it'd be better to just store whether each advancement is on or off I think.
// IDK. This works for now because this is all really just testing display stuff. I guess.
aa::OverlayManager::OverlayManager()
{
    const auto& logger = get_logger("OverlayManager");
    prereqs.yOffset_   = reqs.PADDING;
    reqs.yOffset_      = reqs.TILE_SIZE;

    std::ifstream f("advancements.json");
    if (!f.good()) throw std::runtime_error("No advancements.json found");
    auto advancements = json::parse(f);

    auto& rm = aa::ResourceManager::instance();

    logger.debug("Loading map of available assets.");
    auto assets = aa::ResourceManager::getAllAssets();
    for (auto& [k, v] : assets)
    {
        logger.debug("Asset named ", k, " located at ", v);
    }
    logger.debug("Was able to find ", assets.size(), " potential assets.");

    uint64_t failed_loads = 0;
    if (not reqs.rb_.buf().empty())
    {
        logger.warning("We already have buffered advancement icons..?");
    }
    for (auto& k : advancements)
    {
        logger.debug("Found advancement category: ", k["@name"],
                     ". Now iterating advancements...");
        const std::string cat = k["@name"].get<std::string>();
        for (auto& a : k["advancement"])
        {
            const auto adv_ids = parseID(a);
            this->advancements.emplace_back(cat, adv_ids.full_id, parseIcon(a));
            auto& adv = this->advancements.back();

            // First, load criteria if they exist.
            if (a.contains("criteria"))
            {
                if (not a["criteria"].contains("criterion"))
                {
                    logger.error(
                        adv.name,
                        ": Failed to load criteria - [criteria] does not contain [criterion].");
                }
                else
                {
                    logger.debug("Loading criteria for ", adv.name);
                    for (auto& c : a["criteria"]["criterion"])
                    {
                        // Try to load the criteria. The right way.
                        const auto cid = parseID(c);
                        if (not rm.test_criteria.contains(cid.icon_id))
                        {
                            logger.error("Could not find criteria texture for: ", cid.full_id,
                                         " (ICON ID: ", cid.icon_id, ")");
                        }
                        else
                        {
                            prereqs.rb_.buf().emplace_back(cid.full_id,
                                                           &rm.test_criteria[cid.icon_id]);
                        }
                    }
                }
            }

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
            if (adv_ids.icon_id == "")
            {
                logger.warning("Found an advancement with no name, category: ", adv.category);
                continue;
            }
            if (assets.contains(adv_ids.icon_id))
            {
                reqs.rb_.buf().emplace_back(adv.name,
                                            rm.store_texture_at(assets[adv_ids.icon_id]));
                logger.debug("Loaded implicit icon for ", adv.name,
                             " from file: ", assets[adv_ids.icon_id]);
                continue;
            }
            logger.error("Could not load asset for name: ", adv.name);
            failed_loads += 1;
        }
    }

    logger.info("Properly loaded ", reqs.rb_.buf().size(), " tiles. Failed to load ", failed_loads,
                " advancements.");
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

void aa::OverlayManager::update_with(string_map<::Advancement>& advs)
{
    const auto& logger = get_logger("OverlayManager");
    std::erase_if(reqs.rb_.buf(),
                  [&](const Tile& adv)
                  {
                      logger.debug("Checking if tile with name: ", adv.name, " is completed.");
                      if (advs.contains(adv.name))
                      {
                          logger.debug("Removing tile: ", adv.name);
                          if (advs.at(adv.name).completed) return true;
                      }
                      return false;
                  });
}
