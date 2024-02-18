#include "Advancements.hpp"

#include "logging.hpp"

#include "ResourceManager.hpp"

#include <nlohmann/json.hpp>
using json = nlohmann::json;

namespace aa
{
namespace manifest
{
// This namespace exists to group parsers that only act on
// manifest JSON files (i.e. JSON files that define advancements)
// Specifically the format of file that is generated from the XML.
std::string unprefixed(std::string s)
{
    const std::string_view mc_prefix = "minecraft:";
    if (s.starts_with(mc_prefix))
    {
        return s.substr(mc_prefix.size());
    }
    return s;
}

auto unprefixed_id(auto& j)
{
    // Remove minecraft: prefix from identifier. We don't care about it.
    return unprefixed(j["@id"].template get<std::string>());
}

template <typename T>
auto after_last(auto delim, T iterable)
{
    // now we have a normalized id, something like: end/root or hello/world
    auto it = std::find(iterable.crbegin(), iterable.crend(), delim);
    return T{it.base(), iterable.cend()};
}

auto parse_advancement_id(auto& j)
{
    // Specifically used on manifest load.
    struct ParseResult
    {
        std::string full_id;
        std::string category;
        std::string icon;
    };

    auto id = unprefixed_id(j);

    // now we have a normalized id, something like: end/root or hello/world
    auto it = std::find(id.crbegin(), id.crend(), '/');
    if (it == id.crend()) throw std::runtime_error("Could not parse advancement ID: " + id);

    auto icon     = std::string{it.base(), id.cend()};
    auto category = std::string{id.cbegin(), it.base() - 1};

    return ParseResult{std::move(id), std::move(category), std::move(icon)};
}

std::string get_icon(auto& j)
{
    if (j.contains("@icon")) return j["@icon"].template get<std::string>();
    return "";
}
} // namespace manifest

AdvancementManifest
AdvancementManifest::from_file(std::string_view filename /* advancements.json */)
{
    auto& logger = get_logger("AdvancementManifest::from_file");
    AdvancementManifest ret{};

    logger.debug("Loading core advancement manifest from file: ", filename);

    std::ifstream f(filename.data() /* msvc */);
    if (!f.good())
    {
        logger.error("Failed to load core advancement manifest from file: ", filename);
        throw std::runtime_error("No advancements manifest (json) found / loadable!");
    }

    logger.debug("Parsing manifest JSON.");
    auto advancements = json::parse(f);

    // We load all assets here, during manifest creation.
    // Generally, we only create one manifest. However, we can create more, not sure
    // why we would, but hey. However, let's load everything statically regardless.
    auto& rm = aa::ResourceManager::instance();

    logger.debug("Loading map of available assets.");
    auto assets = aa::ResourceManager::getAllAssets();
    for (auto& [k, v] : assets)
    {
        logger.debug("Asset named ", k, " located at ", v);
    }
    logger.debug("Was able to find ", assets.size(), " potential assets.");

    for (auto& k : advancements)
    {
        /*
         *   ...,  {
         *           "@name": "Adventure",
         *           "advancement": [...]
         */
        const std::string cat = k["@name"];
        logger.debug("Found advancement category: ", cat, ". Now iterating advancements...");
        for (auto& a : k["advancement"])
        {
            /*
             *  ..., {
             *         "@id": "minecraft:adventure/two_birds_one_arrow",
             *         "@name": "Two Birds, One Arrow",
             *         "@short_name": "Two Birds",
             *         "@type": "challenge",
             *         "@half": "false"
             *       },
             */
            const auto id           = manifest::parse_advancement_id(a);
            std::string pretty_name = a["@name"];
            std::string short_name  = get_or(a, "@short_name", pretty_name);
            logger.debug("Got ID: ", id.full_id, " (category: ", id.category,
                         ", icon name: ", id.icon, ") actual name: ", pretty_name,
                         " short name: ", short_name);
            // "minecraft:adventure/two_birds_one_arrow"
            // -> ["adventure/two_birds_one_arrow", "two_birds_one_arrow"]
            auto [itr, success] = ret.advancements.emplace(
                id.full_id, Advancement{id.icon, id.category, pretty_name, short_name});
            if (!success)
            {
                logger.warning("Failed to insert advancement: ", id.full_id);
            }
            auto& adv = itr->second;

            // Load all criteria.
            if (a.contains("criteria"))
            {
                if (not a["criteria"].contains("criterion"))
                {
                    logger.fatal_error(adv.full_id(),
                                       ": Failed to load criteria: key 'criterion' missing.");
                }

                // Load criteria.
                logger.debug("Loading criteria for ", adv.name);
                for (auto& c : a["criteria"]["criterion"])
                {
                    // Try to load the criteria. The right way.
                    const auto crit = manifest::unprefixed_id(c);
                    const std::string icon =
                        c.contains("@icon") ? c["@icon"].template get<std::string>() : crit;
                    logger.debug("Adding criterion: ", crit);
                    if (not rm.criteria_map.contains(icon))
                    {
                        if (not rm.criteria_map.contains(crit))
                        {
                            logger.fatal_error("Could not find criteria texture for: ", icon,
                                               " (In advancement: ", adv.full_id(), ")");
                        }
                        adv.add_criteria(crit, rm.criteria_map[crit].get());
                        continue;
                    }
                    adv.add_criteria(crit, rm.criteria_map[icon].get());
                }
            }

            // Now all that is missing is our own icon.

            // Load EXPLICIT icon.
            if (const auto explicit_icon = manifest::get_icon(a); explicit_icon != "")
            {
                if (not assets.contains(explicit_icon))
                {
                    logger.fatal_error("Could not locate icon: ", explicit_icon, " (for ",
                                       adv.full_id(), ")");
                }

                adv.icon = rm.store_texture_at(assets[explicit_icon]);
                logger.debug("Loaded explicit icon for ", adv.name,
                             " from file: ", assets[explicit_icon]);

                continue;
            }

            // Load IMPLICIT icon.
            if (assets.contains(adv.name))
            {
                adv.icon = rm.store_texture_at(assets[adv.name]);
                logger.debug("Loaded implicit icon for ", adv.name,
                             " from file: ", assets[adv.name]);

                continue;
            }

            // Error out if we don't get it.
            logger.fatal_error("Could not load asset for name: ", adv.name);
        }
    }

    logger.info("Loaded ", ret.advancements.size(), " advancements.");

    return ret;
}

AdvancementStatus AdvancementStatus::from_file(std::string_view filename,
                                               const AdvancementManifest& manifest)
{
    auto& logger = get_logger("AdvancementStatus::from_file");
    AdvancementStatus ret{};

    logger.debug("Loading advancements from file: ", filename);

    std::ifstream f(filename.data() /* msvc */);
    if (!f.good())
    {
        logger.error("Could not open file: ", filename);
        ret.meta.valid = false;
        return ret;
    }

    auto advancements = json{};
    try
    {
        advancements = json::parse(f);
    }
    catch (std::exception& e)
    {
        logger.error("Failed to parse JSON: ", e.what());
        ret.meta.valid = false;
        return ret;
    }

    // Copy :sob:
    // Kind of have to do this. Because we are 'removing' things,
    // not 'adding' them. Oh well.
    ret.incomplete = manifest.advancements;

    const std::string_view adv_prefix = "minecraft:";
    for (const auto& [key, value] : advancements.items())
    {
        if (key.starts_with("minecraft:recipes/") or not key.starts_with(adv_prefix)) continue;

        logger.debug("Found advancement: ", key);
        // this is probably definitely totally an advancement :)
        auto name = key.substr(adv_prefix.size());
        if (not ret.incomplete.contains(name))
        {
            logger.error("We don't have the advancement: ", name);
            ret.meta.valid = false;
            return ret;
        }

        const auto is_completed = value.contains("done") && value["done"].template get<bool>();

        if (is_completed)
        {
            // Basically, move from incomplete -> complete.
            ret.complete.emplace(name, std::move(ret.incomplete[name]));
            ret.incomplete.erase(name);

            // Don't need to do anything else. No criteria, etc.
            continue;
        }

        if (value.contains("criteria"))
        {
            // Okay, so, this is an incomplete criteria.
            // So what we need is the criteria that do NOT exist.
            // The easiest way to do this is to just copy over
            // the entire "criteria for this thing" list then
            // remove the ones we actually have while iterating.
            // Same way that the overall system works.

            // Okay, actually we DON'T have to copy anything over.
            // Because... we already did. When we grabbed manifest
            // advancements into our ret->advancements.

            auto& adv = ret.incomplete[name];
            for (const auto& itr : value["criteria"].items())
            {
                // These are the ones that actually exist.
                auto crit_key = manifest::unprefixed(itr.key());

                if (not adv.criteria.contains(crit_key))
                {
                    // This is fine actually. We will get random
                    // criteria from parsing that aren't 'real'
                    // criteria. If they're not in manifest,
                    // just ignore them. Not real! Fake! !!!!!
                    // logger.fatal_error("Criteria key ", crit_key,
                    //                   " is not present in advancement ", name);
                    continue;
                }
                adv.criteria_ordered.erase(
                    std::find(adv.criteria_ordered.begin(), adv.criteria_ordered.end(), crit_key));
                adv.criteria.erase(crit_key);
            }
        }
    }

    return ret;
}

AdvancementStatus AdvancementStatus::from_default(const AdvancementManifest& manifest)
{
    AdvancementStatus ret{};
    ret.incomplete = manifest.advancements;
    return ret;
}
} // namespace aa