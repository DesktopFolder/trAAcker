#include "Overlay.hpp"
#include "Advancements.hpp"
#include "logging.hpp"

#include <fstream>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

// Current opinion on this code - not very good
// Seems ridiculously stateful and over the top for performance when it's not really...
// like it'd be better to just store whether each advancement is on or off I think.
// IDK. This works for now because this is all really just testing display stuff. I guess.
aa::OverlayManager::OverlayManager(const AdvancementManifest& manifest)
{
    prereqs.yOffset_ = reqs.PADDING;
    reqs.yOffset_    = reqs.TILE_SIZE;

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
