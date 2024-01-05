#include "Overlay.hpp"
#include "Advancements.hpp"
#include "ConfigProvider.hpp"
#include "logging.hpp"

#include <fstream>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

aa::OverlayManager::OverlayManager(const AdvancementManifest& manifest,
                                   const nlohmann::json& config)
{
    // Configure the overlay.
    using aa::conf::get_or;
    using aa::conf::apply;

    // Set the tile sizes. This is specifically the size of the inner sprite in the
    // case of the major advancements. For the criteria, it's just... the size.
    aa::conf::apply(config, "criteria-size", [&](uint64_t sz){ prereqs.set_size(sz); });
    aa::conf::apply(config, "advancement-size", [&](uint64_t sz){ reqs.set_size(sz); });

    aa::conf::apply(config, "criteria-padding", [&](uint64_t sz){ prereqs.set_padding(sz); });
    aa::conf::apply(config, "advancement-padding", [&](uint64_t sz){ reqs.set_padding(sz); });

    // The two lines of overlay both have Y offsets. We have a default, but this works.
    prereqs.yOffset_ = get_or(config, "criteria-y", reqs.get_padding());
    reqs.yOffset_    = get_or(config, "advancement-y", prereqs.get_size());
    aa::conf::apply(config, "rate", [&](int rate) { setRate(static_cast<uint8_t>(rate)); });

    // Okay, now to test out the new advancements setup.
    auto status = AdvancementStatus::from_default(manifest);

    reset_from_status(status);
}

void aa::OverlayManager::reset_from_status(const AdvancementStatus& status)
{
    if (!status.meta.valid)
    {
        get_logger("OverlayManager").warning("Failed to reset from status - parse/load error.");
        return;
    }

    reqs.clear();
    prereqs.clear();

    for (auto& [k, v] : status.incomplete)
    {
        reqs.emplace(k, v.icon);

        for (auto& [ck, cv] : v.criteria)
        {
            prereqs.emplace(ck, cv);
        }
    }
}

void aa::OverlayManager::reset_from_file(std::string_view filename,
                                         const AdvancementManifest& manifest)
{
    // Okay, now to test out the new advancements setup.
    auto status = AdvancementStatus::from_file(filename, manifest);

    get_logger("OverlayManager")
        .debug("Resetting from advancements.json manifest given file ", filename);

    // This can handle errors, we assume that sometimes we get a bad file.
    reset_from_status(status);
}

void aa::OverlayManager::reset(const AdvancementManifest& manifest)
{
    // Okay, now to test out the new advancements setup.
    auto status = AdvancementStatus::from_default(manifest);

    get_logger("OverlayManager").debug("Resetting from advancements.json manifest.");

    reset_from_status(status);
}

void aa::OverlayManager::debug()
{
    auto& logger = get_logger("OverlayManager::debug");

    logger.debug("Major requirements buffer size: ", reqs.rb_.buf().size());
    logger.debug("Pre-requirements buffer size: ", prereqs.rb_.buf().size());
}
