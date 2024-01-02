#include "Overlay.hpp"
#include "Advancements.hpp"
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
    prereqs.yOffset_ = reqs.PADDING;
    reqs.yOffset_    = reqs.TILE_SIZE;

    // Okay, now to test out the new advancements setup.
    auto manifest = AdvancementManifest::from_file("advancements.json");
    auto status   = AdvancementStatus::from_default(manifest);

    reset_from_status(status);
}

void aa::OverlayManager::reset_from_status(const AdvancementStatus& status)
{
    reqs.rb_.buf().clear();
    prereqs.rb_.buf().clear();

    for (auto& [k, v] : status.incomplete)
    {
        reqs.rb_.buf().emplace_back(k, v.icon);

        for (auto& [ck, cv] : v.criteria)
        {
            prereqs.rb_.buf().emplace_back(ck, cv);
        }
    }
}

void aa::OverlayManager::reset_from_file(std::string_view filename)
{
    // Okay, now to test out the new advancements setup.
    auto manifest = AdvancementManifest::from_file("advancements.json");
    auto status   = AdvancementStatus::from_file(filename, manifest);

    get_logger("OverlayManager")
        .debug("Resetting from advancements.json manifest given file ", filename);

    reset_from_status(status);
}

void aa::OverlayManager::reset()
{
    // Okay, now to test out the new advancements setup.
    auto manifest = AdvancementManifest::from_file("advancements.json");
    auto status   = AdvancementStatus::from_default(manifest);

    get_logger("OverlayManager").debug("Resetting from advancements.json manifest.");

    reset_from_status(status);
}
