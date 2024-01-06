#pragma once

// What is the overlay?
// It's basically a set of turntables.
// Oh, how the turn tables.
#include "TurnTable.hpp"
#include "ResourceManager.hpp"
#include "Advancements.hpp"
#include "utilities.hpp"

#include <nlohmann/json_fwd.hpp>

#include <vector>

namespace aa
{
struct Advancement {
    // let's not care too much atm
    Advancement(std::string c, std::string n, std::string i) : category(c), name(n), icon(i) {}
    std::string category;
    std::string name;
    std::string icon;
};

struct Criteria {
    Criteria(std::string c, std::string n, std::string i) : advancement(c), name(n), icon(i) {}
    std::string advancement;
    std::string name;
    std::string icon;
};

struct OverlayManager
{
    TurnTable prereqs;
    TurnTable reqs;
    sf::Texture testText;

    std::vector<Advancement> advancements;
    std::vector<Criteria> criteria;

    OverlayManager(AdvancementManifest& manifest, const nlohmann::json& config);

    // assumes manifest is in advancements.json atm
    void reset_from_file(std::string_view filename, const AdvancementManifest& manifest);

    void reset_from_status(const AdvancementStatus& status);

    void reset(const AdvancementManifest& manifest);

    void debug();

    void setRate(uint8_t rate)
    {
        prereqs.rate_ = rate;
        reqs.rate_    = rate;
    }

    void render(sf::RenderWindow& win)
    {
        prereqs.animateDraw(win);
        reqs.animateDraw(win);
    }

    void remap_textures(AdvancementManifest& manifest);
};
}  // namespace aa
