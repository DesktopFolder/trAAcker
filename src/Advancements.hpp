#pragma once

#include <string>
#include <vector>
#include <SFML/Graphics/Texture.hpp>

#include "utilities.hpp"

namespace aa
{
struct Advancement
{
    /* Advancement - The canonical type.
     *
     * "minecraft:adventure/adventuring_time" ->
     * category: adventure
     * name: adventuring_time
     * criteria: [ "plains", "wooded_hills", ... ]
     *   -> Criteria can have a custom name.
     * icon: adventuring_time.png -> loaded into sf::Texture
     */
    std::string name;
    std::string category;
    std::string pretty_name;
    std::string short_name;

    std::string full_id() const { return category + "/" + name; }

    // Just do this for now. Deal with names later.
    string_map<const sf::Texture*> criteria;
    std::vector<std::string> criteria_ordered;

    void add_criteria(std::string key, const sf::Texture* t)
    {
        criteria.emplace(key, t);
        criteria_ordered.push_back(key);
    }

    const sf::Texture* icon{};
};

/* AdvancementManifest
 * Gives information on the possible/total advancements.
 * Parsed by AdvancementStatus + the current Advancement file.
 */
struct AdvancementManifest
{
    static AdvancementManifest from_file(std::string_view filename);

    string_map<Advancement> advancements;
};

/* AdvancementStatus
 * Basically, we need the following information:
 * - advancements.json -> all advancements and criteria
 *   - also all texture information
 * - player's advancements.json -> current advancements
 * - implicit: 'which advancements are incomplete'
 */
struct AdvancementStatus
{
    static AdvancementStatus from_file(std::string_view filename, const AdvancementManifest&);
    static AdvancementStatus from_default(const AdvancementManifest&);

    // Just the basics. Advancements / Tier 1 criteria that are complete/incomplete.
    string_map<Advancement> incomplete;
    string_map<Advancement> complete;

    // Metainformation that we gather as we parse. All custom, because we don't want
    // to store and search through absolutely everything.
    struct
    {
        bool has_egap = false;
        bool valid    = true;
    } meta{};
};
} // namespace aa