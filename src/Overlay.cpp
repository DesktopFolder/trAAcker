#include "Overlay.hpp"
#include "Advancements.hpp"
#include "ConfigProvider.hpp"
#include "logging.hpp"

#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace aa
{
OverlayManager::OverlayManager(AdvancementManifest& manifest)
{
    // Configure the overlay.
    using aa::conf::apply;
    using aa::conf::get_or;

    const auto& config = aa::conf::getNS("overlay");

    // Set the tile sizes. This is specifically the size of the inner sprite in the
    // case of the major advancements. For the criteria, it's just... the size.
    aa::conf::apply(config, "criteria-size", [&](uint64_t sz) { prereqs.set_size(sz); });
    aa::conf::apply(config, "advancement-size", [&](uint64_t sz) { reqs.set_size(sz); });

    aa::conf::apply(config, "criteria-padding", [&](uint64_t sz) { prereqs.set_padding(sz); });
    aa::conf::apply(config, "advancement-padding", [&](uint64_t sz) { reqs.set_padding(sz); });

    // The two lines of overlay both have Y offsets. We have a default, but this works.
    prereqs.yOffset_ = get_or(config, "criteria-y", reqs.get_padding());
    reqs.yOffset_    = get_or(config, "advancement-y", prereqs.get_size() + prereqs.yOffset_);

    prereqs.xOffset_ = get_or(config, "criteria-x", prereqs.xOffset_);
    reqs.xOffset_    = get_or(config, "advancement-x", prereqs.xOffset_);

    reqs.drawText = not get_or(config, "disable-advancement-text", false);
    reqs.fontSize = get_or(config, "advancement-font-size", reqs.fontSize);

    aa::conf::apply(config, "rate", [&](int rate) { setRate(static_cast<uint8_t>(rate)); });

    get_logger("OverlayManager")
        .debug("Created OverlayManager. Current configuration:")
        .debug("Criteria Size: ", prereqs.get_size())
        .debug("Criteria Y: ", prereqs.yOffset_)
        .debug("Advancements Y: ", reqs.yOffset_)
        .debug("Now remapping textures as appropriate.");

    remap_textures(manifest);

    // Okay, now to test out the new advancements setup.
    auto status = AdvancementStatus::from_default(manifest);

    reset_from_status(status);
}

void OverlayManager::remap_textures(AdvancementManifest& manifest)
{
    auto& rm = aa::ResourceManager::instance();
    // This works for now :)

    const auto crit_sz = prereqs.get_texture_size();
    const auto adv_sz  = reqs.get_texture_size();
    get_logger("OverlayManager")
        .debug("Remapping all textures.")
        .debug("Remapping criteria to size: ", crit_sz)
        .debug("Remapping advancements to size: ", adv_sz);

    for (auto& [_, advancement] : manifest.advancements)
    {
        advancement.icon = rm.remap_texture(advancement.icon, adv_sz);
        for (auto& itr : advancement.criteria)
        {
            itr.second = rm.remap_texture(itr.second, crit_sz);
        }
    }
}

void OverlayManager::reset_from_status(const AdvancementStatus& status)
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
        reqs.emplace(v.pretty_name, v.icon);

        for (auto loc : v.criteria_ordered)
        {
            prereqs.emplace("", v.criteria.find(loc)->second);
        }
    }
}

void OverlayManager::reset_from_file(std::string_view filename,
                                         const AdvancementManifest& manifest)
{
    // Okay, now to test out the new advancements setup.
    auto status = AdvancementStatus::from_file(filename, manifest);

    get_logger("OverlayManager")
        .debug("Resetting from advancements.json manifest given file ", filename);

    // This can handle errors, we assume that sometimes we get a bad file.
    reset_from_status(status);
}

void OverlayManager::reset(const AdvancementManifest& manifest)
{
    // Okay, now to test out the new advancements setup.
    auto status = AdvancementStatus::from_default(manifest);

    get_logger("OverlayManager").debug("Resetting from advancements.json manifest.");

    reset_from_status(status);
}

void OverlayManager::debug()
{
    auto& logger = get_logger("OverlayManager::debug");

    logger.debug("Major requirements buffer size: ", reqs.rb_.buf().size());
    logger.debug("Pre-requirements buffer size: ", prereqs.rb_.buf().size());
}
} // namespace aa