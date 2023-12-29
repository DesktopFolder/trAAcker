#include "Advancements.hpp"

#include "logging.hpp"
#include <nlohmann/json.hpp>
using json = nlohmann::json;

string_map<Advancement> get_advancements(std::string_view filename)
{
    auto& logger = get_logger("get_advancements");
    logger.debug("Getting advancements from file: ", filename);

    std::ifstream f(filename);
    if (!f.good())
    {
        logger.error("Could not open file: ", filename);
        throw std::runtime_error("No advancements.json found");
    }
    const auto advancements = json::parse(f);

    string_map<Advancement> ret;
    const std::string_view adv_prefix = "minecraft:";
    for (const auto& [key, value] : advancements.items())
    {
        if (key.starts_with("minecraft:recipes/") or not key.starts_with(adv_prefix)) continue;

        logger.debug("Found advancement: ", key);
        // this is probably definitely totally an advancement :)
        auto name = key.substr(adv_prefix.size());

        auto [adv, _] = ret.emplace(name, Advancement{});

        if (value.contains("done")) {
            adv->second.completed = value["done"].template get<bool>();
        }
        if (value.contains("criteria"))
        {
            for (const auto& itr : value["criteria"].items())
            {
                adv->second.completed_criteria.emplace_back(itr.key());
            }
        }
    }
    return ret;
}
