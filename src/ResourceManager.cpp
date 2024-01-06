#include "ResourceManager.hpp"

#include "logging.hpp"

#include <filesystem>
#include <SFML/Graphics.hpp>

aa::ResourceManager& aa::ResourceManager::instance()
{
    static aa::ResourceManager mgr{};
    return mgr;
}

void asset_helper(auto& dir_entry, auto& assets)
{
    if (!dir_entry.is_regular_file()) return;
    const auto p = dir_entry.path();
    const auto s = p.string();
    if (not(s.ends_with(".png") or s.ends_with(".gif"))) return;
    if (assets.contains(aa::ResourceManager::assetName(s)))
    {
        // we have this asset loaded already
        // continue unless this is HIGH RESOLUTION OOOO
        if (s.size() < 3 || s[s.size() - 3] != '^') return;
    }
    assets.emplace(aa::ResourceManager::assetName(s), s);
}

std::unordered_map<std::string, std::string> aa::ResourceManager::getAllAssets()
{
    namespace fs = std::filesystem;

    std::unordered_map<std::string, std::string> assets;

    for (const fs::directory_entry& dir_entry :
         fs::recursive_directory_iterator("assets/sprites/global/"))
    {
        asset_helper(dir_entry, assets);
    }
    for (const fs::directory_entry& dir_entry :
         fs::recursive_directory_iterator("assets/sprites/gif/"))
    {
        asset_helper(dir_entry, assets);
    }

    return assets;
}

const sf::Texture* aa::ResourceManager::remap_texture(const sf::Texture* base, uint64_t new_size)
{
    const auto [x, y] = base->getSize();
    if (x == new_size)
    {
        // No need to remap here.
        return base;
    }
    sf::Sprite s;
    // Say, 32x32
    float initial = static_cast<float>(x);
    // Say, 48x48
    float desired = static_cast<float>(new_size);
    // Say, 48 / 32 = 1.5
    float factor = desired / initial;
    s.setScale(factor, factor);
    s.setTexture(*base);

    random_rerenders.emplace_back(std::make_unique<sf::RenderTexture>());
    auto& t = *random_rerenders.back().get();
    if (not t.create(new_size, new_size))
    {
        // This will likely crash us, but I mean. Um. It's a problem.
        get_logger("ResourceManager").error("Failed to create a render texture of size ", new_size, "!!!");
        return nullptr;
    }
    t.draw(s);
    t.display();

    return &t.getTexture();
}

aa::ResourceManager::~ResourceManager() {}

void aa::ResourceManager::loadCriteria(std::string path, std::string name)
{
    // this is all startup stuff so don't worry about it
    sf::Texture text;
    text.loadFromFile(path);
    current_->emplace(name, text);
    test_criteria.emplace(name, text);

    auto t = std::make_unique<sf::Texture>();
    t->loadFromFile(path);
    criteria_map.emplace(name, std::move(t));
}

void aa::ResourceManager::loadAllCriteria()
{
    criteria["animals"] = {};
    current_            = &criteria["animals"];
    // loadCriteria("assets/sprites/global/criteria/animals/axolotl.png", "axolotl");
    loadCriteria("assets/sprites/global/criteria/animals/bee.png", "bee");
    // loadCriteria("assets/sprites/global/criteria/animals/camel.png", "camel");
    loadCriteria("assets/sprites/global/criteria/animals/cat.png", "cat");
    loadCriteria("assets/sprites/global/criteria/animals/chicken.png", "chicken");
    loadCriteria("assets/sprites/global/criteria/animals/cow.png", "cow");
    loadCriteria("assets/sprites/global/criteria/animals/donkey.png", "donkey");
    loadCriteria("assets/sprites/global/criteria/animals/fox.png", "fox");
    // loadCriteria("assets/sprites/global/criteria/animals/frog.png", "frog");
    // loadCriteria("assets/sprites/global/criteria/animals/goat.png", "goat");
    loadCriteria("assets/sprites/global/criteria/animals/horse.png", "horse");
    loadCriteria("assets/sprites/global/criteria/animals/llama.png", "llama");
    loadCriteria("assets/sprites/global/criteria/animals/mooshroom.png", "mooshroom");
    loadCriteria("assets/sprites/global/criteria/animals/mule.png", "mule");
    loadCriteria("assets/sprites/global/criteria/animals/ocelot.png", "ocelot");
    loadCriteria("assets/sprites/global/criteria/animals/panda.png", "panda");
    loadCriteria("assets/sprites/global/criteria/animals/pig.png", "pig");
    loadCriteria("assets/sprites/global/criteria/animals/polar_bear.png", "polar_bear");
    loadCriteria("assets/sprites/global/criteria/animals/rabbit.png", "rabbit");
    loadCriteria("assets/sprites/global/criteria/animals/sheep.png", "sheep");
    loadCriteria("assets/sprites/global/criteria/animals/sniffer.png", "sniffer");
    loadCriteria("assets/sprites/global/criteria/animals/strider.png", "strider");
    loadCriteria("assets/sprites/global/criteria/animals/turtle.png", "turtle");
    loadCriteria("assets/sprites/global/criteria/animals/wolf.png", "wolf");
    criteria["biomes"] = {};
    current_           = &criteria["biomes"];
    loadCriteria("assets/sprites/global/criteria/biomes/badlands_plateau.png", "badlands_plateau");
    loadCriteria("assets/sprites/global/criteria/biomes/badlands.png", "badlands");
    loadCriteria("assets/sprites/global/criteria/biomes/bamboo_jungle_hills.png",
                 "bamboo_jungle_hills");
    loadCriteria("assets/sprites/global/criteria/biomes/bamboo_jungle.png", "bamboo_jungle");
    loadCriteria("assets/sprites/global/criteria/biomes/basalt_deltas.png", "basalt_deltas");
    loadCriteria("assets/sprites/global/criteria/biomes/beach.png", "beach");
    loadCriteria("assets/sprites/global/criteria/biomes/birch_forest_hills.png",
                 "birch_forest_hills");
    loadCriteria("assets/sprites/global/criteria/biomes/birch_forest.png", "birch_forest");
    loadCriteria("assets/sprites/global/criteria/biomes/cold_ocean.png", "cold_ocean");
    loadCriteria("assets/sprites/global/criteria/biomes/crimson_forest.png", "crimson_forest");
    loadCriteria("assets/sprites/global/criteria/biomes/dark_forest.png", "dark_forest");
    loadCriteria("assets/sprites/global/criteria/biomes/deep_cold_ocean.png", "deep_cold_ocean");
    loadCriteria("assets/sprites/global/criteria/biomes/deep_frozen_ocean.png",
                 "deep_frozen_ocean");
    loadCriteria("assets/sprites/global/criteria/biomes/deep_lukewarm_ocean.png",
                 "deep_lukewarm_ocean");
    loadCriteria("assets/sprites/global/criteria/biomes/deep_ocean.png", "deep_ocean");
    loadCriteria("assets/sprites/global/criteria/biomes/desert_hills.png", "desert_hills");
    loadCriteria("assets/sprites/global/criteria/biomes/desert_lakes.png", "desert_lakes");
    loadCriteria("assets/sprites/global/criteria/biomes/desert.png", "desert");
    loadCriteria("assets/sprites/global/criteria/biomes/forest_hills.png", "forest_hills");
    loadCriteria("assets/sprites/global/criteria/biomes/forest.png", "forest");
    loadCriteria("assets/sprites/global/criteria/biomes/frozen_river.png", "frozen_river");
    loadCriteria("assets/sprites/global/criteria/biomes/giant_tree_taiga_hills.png",
                 "giant_tree_taiga_hills");
    loadCriteria("assets/sprites/global/criteria/biomes/giant_tree_taiga.png", "giant_tree_taiga");
    loadCriteria("assets/sprites/global/criteria/biomes/jungle_edge.png", "jungle_edge");
    loadCriteria("assets/sprites/global/criteria/biomes/jungle_hills.png", "jungle_hills");
    loadCriteria("assets/sprites/global/criteria/biomes/jungle.png", "jungle");
    loadCriteria("assets/sprites/global/criteria/biomes/lukewarm_ocean.png", "lukewarm_ocean");
    loadCriteria("assets/sprites/global/criteria/biomes/mountains.png", "mountains");
    loadCriteria("assets/sprites/global/criteria/biomes/mushroom_field_shore.png",
                 "mushroom_field_shore");
    loadCriteria("assets/sprites/global/criteria/biomes/mushroom_fields.png", "mushroom_fields");
    loadCriteria("assets/sprites/global/criteria/biomes/nether_wastes.png", "nether_wastes");
    loadCriteria("assets/sprites/global/criteria/biomes/ocean.png", "ocean");
    loadCriteria("assets/sprites/global/criteria/biomes/plains.png", "plains");
    loadCriteria("assets/sprites/global/criteria/biomes/river.png", "river");
    loadCriteria("assets/sprites/global/criteria/biomes/savanna_plateau.png", "savanna_plateau");
    loadCriteria("assets/sprites/global/criteria/biomes/savanna.png", "savanna");
    loadCriteria("assets/sprites/global/criteria/biomes/snowy_beach.png", "snowy_beach");
    loadCriteria("assets/sprites/global/criteria/biomes/snowy_mountains.png", "snowy_mountains");
    loadCriteria("assets/sprites/global/criteria/biomes/snowy_taiga_hills.png",
                 "snowy_taiga_hills");
    loadCriteria("assets/sprites/global/criteria/biomes/snowy_taiga.png", "snowy_taiga");
    loadCriteria("assets/sprites/global/criteria/biomes/snowy_tundra.png", "snowy_tundra");
    loadCriteria("assets/sprites/global/criteria/biomes/soul_sand_valley.png", "soul_sand_valley");
    loadCriteria("assets/sprites/global/criteria/biomes/stone_shore.png", "stone_shore");
    loadCriteria("assets/sprites/global/criteria/biomes/swamp.png", "swamp");
    loadCriteria("assets/sprites/global/criteria/biomes/taiga_hills.png", "taiga_hills");
    loadCriteria("assets/sprites/global/criteria/biomes/taiga.png", "taiga");
    loadCriteria("assets/sprites/global/criteria/biomes/warm_ocean.png", "warm_ocean");
    loadCriteria("assets/sprites/global/criteria/biomes/warped_forest.png", "warped_forest");
    loadCriteria("assets/sprites/global/criteria/biomes/wooded_badlands_plateau.png",
                 "wooded_badlands_plateau");
    loadCriteria("assets/sprites/global/criteria/biomes/wooded_hills.png", "wooded_hills");
    loadCriteria("assets/sprites/global/criteria/biomes/wooded_mountains.png", "wooded_mountains");
    criteria["cats"] = {};
    current_         = &criteria["cats"];
    // Why Minecraft. Why.
    // TODO - we can actually fix this. CTM's asset map properly maps these assets in @icon. For
    // now, whatever, this system works. Later we should do better asset management...
    loadCriteria("assets/sprites/global/criteria/cats/black.png",
                 "textures/entity/cat/all_black.png");
    loadCriteria("assets/sprites/global/criteria/cats/british_shorthair.png",
                 "textures/entity/cat/british_shorthair.png");
    loadCriteria("assets/sprites/global/criteria/cats/calico.png",
                 "textures/entity/cat/calico.png");
    loadCriteria("assets/sprites/global/criteria/cats/jellie.png",
                 "textures/entity/cat/jellie.png");
    // loadCriteria("assets/sprites/global/criteria/cats/ocelot.png",
    // "textures/entity/cat/ocelot.png");
    loadCriteria("assets/sprites/global/criteria/cats/persian.png",
                 "textures/entity/cat/persian.png");
    loadCriteria("assets/sprites/global/criteria/cats/ragdoll.png",
                 "textures/entity/cat/ragdoll.png");
    loadCriteria("assets/sprites/global/criteria/cats/red.png", "textures/entity/cat/red.png");
    loadCriteria("assets/sprites/global/criteria/cats/siamese.png",
                 "textures/entity/cat/siamese.png");
    loadCriteria("assets/sprites/global/criteria/cats/tabby.png", "textures/entity/cat/tabby.png");
    loadCriteria("assets/sprites/global/criteria/cats/tuxedo.png",
                 "textures/entity/cat/black.png");
    loadCriteria("assets/sprites/global/criteria/cats/white.png", "textures/entity/cat/white.png");
    criteria["food"] = {};
    current_         = &criteria["food"];
    loadCriteria("assets/sprites/global/criteria/food/apple.png", "apple");
    loadCriteria("assets/sprites/global/criteria/food/baked_potato.png", "baked_potato");
    loadCriteria("assets/sprites/global/criteria/food/beef.png", "beef");
    loadCriteria("assets/sprites/global/criteria/food/beetroot.png", "beetroot");
    loadCriteria("assets/sprites/global/criteria/food/beetroot_soup.png", "beetroot_soup");
    loadCriteria("assets/sprites/global/criteria/food/bread.png", "bread");
    loadCriteria("assets/sprites/global/criteria/food/carrot.png", "carrot");
    loadCriteria("assets/sprites/global/criteria/food/chorus_fruit.png", "chorus_fruit");
    loadCriteria("assets/sprites/global/criteria/food/cod.png", "cod");
    loadCriteria("assets/sprites/global/criteria/food/cooked_beef.png", "cooked_beef");
    loadCriteria("assets/sprites/global/criteria/food/cooked_chicken.png", "cooked_chicken");
    loadCriteria("assets/sprites/global/criteria/food/cooked_cod.png", "cooked_cod");
    loadCriteria("assets/sprites/global/criteria/food/cooked_mutton.png", "cooked_mutton");
    loadCriteria("assets/sprites/global/criteria/food/cooked_porkchop.png", "cooked_porkchop");
    loadCriteria("assets/sprites/global/criteria/food/cooked_rabbit.png", "cooked_rabbit");
    loadCriteria("assets/sprites/global/criteria/food/cooked_salmon.png", "cooked_salmon");
    loadCriteria("assets/sprites/global/criteria/food/cookie.png", "cookie");
    loadCriteria("assets/sprites/global/criteria/food/dried_kelp.png", "dried_kelp");
    loadCriteria("assets/sprites/global/criteria/food/enchanted_golden_apple$825.png",
                 "enchanted_golden_apple");
    // loadCriteria("assets/sprites/global/criteria/food/glow_berries.png", "glow_berries");
    loadCriteria("assets/sprites/global/criteria/food/golden_apple.png", "golden_apple");
    loadCriteria("assets/sprites/global/criteria/food/golden_carrot.png", "golden_carrot");
    loadCriteria("assets/sprites/global/criteria/food/honey_bottle.png", "honey_bottle");
    loadCriteria("assets/sprites/global/criteria/food/melon_slice.png", "melon_slice");
    loadCriteria("assets/sprites/global/criteria/food/mushroom_stew.png", "mushroom_stew");
    loadCriteria("assets/sprites/global/criteria/food/mutton.png", "mutton");
    loadCriteria("assets/sprites/global/criteria/food/poisonous_potato.png", "poisonous_potato");
    loadCriteria("assets/sprites/global/criteria/food/porkchop.png", "porkchop");
    loadCriteria("assets/sprites/global/criteria/food/potato.png", "potato");
    loadCriteria("assets/sprites/global/criteria/food/pufferfish.png", "pufferfish");
    loadCriteria("assets/sprites/global/criteria/food/pumpkin_pie.png", "pumpkin_pie");
    loadCriteria("assets/sprites/global/criteria/food/rabbit_stew.png", "rabbit_stew");
    loadCriteria("assets/sprites/global/criteria/food/raw_chicken.png", "raw_chicken");
    loadCriteria("assets/sprites/global/criteria/food/raw_rabbit.png", "raw_rabbit");
    loadCriteria("assets/sprites/global/criteria/food/rotten_flesh.png", "rotten_flesh");
    loadCriteria("assets/sprites/global/criteria/food/salmon.png", "salmon");
    loadCriteria("assets/sprites/global/criteria/food/spider_eye.png", "spider_eye");
    loadCriteria("assets/sprites/global/criteria/food/suspicious_stew.png", "suspicious_stew");
    loadCriteria("assets/sprites/global/criteria/food/sweet_berries.png", "sweet_berries");
    loadCriteria("assets/sprites/global/criteria/food/tropical_fish.png", "tropical_fish");
    criteria["mobs"] = {};
    current_         = &criteria["mobs"];
    loadCriteria("assets/sprites/global/criteria/mobs/blaze.png", "blaze");
    loadCriteria("assets/sprites/global/criteria/mobs/cave_spider.png", "cave_spider");
    loadCriteria("assets/sprites/global/criteria/mobs/creeper.png", "creeper");
    loadCriteria("assets/sprites/global/criteria/mobs/drowned.png", "drowned");
    loadCriteria("assets/sprites/global/criteria/mobs/elder_guardian.png", "elder_guardian");
    loadCriteria("assets/sprites/global/criteria/mobs/ender_dragon.png", "ender_dragon");
    loadCriteria("assets/sprites/global/criteria/mobs/enderman.png", "enderman");
    loadCriteria("assets/sprites/global/criteria/mobs/endermite.png", "endermite");
    loadCriteria("assets/sprites/global/criteria/mobs/evoker.png", "evoker");
    loadCriteria("assets/sprites/global/criteria/mobs/ghast.png", "ghast");
    loadCriteria("assets/sprites/global/criteria/mobs/guardian.png", "guardian");
    loadCriteria("assets/sprites/global/criteria/mobs/hoglin.png", "hoglin");
    loadCriteria("assets/sprites/global/criteria/mobs/husk.png", "husk");
    loadCriteria("assets/sprites/global/criteria/mobs/magma_cube.png", "magma_cube");
    loadCriteria("assets/sprites/global/criteria/mobs/phantom.png", "phantom");
    loadCriteria("assets/sprites/global/criteria/mobs/piglin_brute.png", "piglin_brute");
    loadCriteria("assets/sprites/global/criteria/mobs/piglin.png", "piglin");
    loadCriteria("assets/sprites/global/criteria/mobs/pillager.png", "pillager");
    loadCriteria("assets/sprites/global/criteria/mobs/ravager.png", "ravager");
    loadCriteria("assets/sprites/global/criteria/mobs/shulker.png", "shulker");
    // loadCriteria("assets/sprites/global/criteria/mobs/silverfish^16.png", "silverfish^16");
    loadCriteria("assets/sprites/global/criteria/mobs/silverfish^48.png", "silverfish");
    loadCriteria("assets/sprites/global/criteria/mobs/skeleton.png", "skeleton");
    loadCriteria("assets/sprites/global/criteria/mobs/slime.png", "slime");
    loadCriteria("assets/sprites/global/criteria/mobs/spider.png", "spider");
    loadCriteria("assets/sprites/global/criteria/mobs/stray.png", "stray");
    // loadCriteria("assets/sprites/global/criteria/mobs/vex_1.19.3.png", "vex_1");
    loadCriteria("assets/sprites/global/criteria/mobs/vex.png", "vex");
    loadCriteria("assets/sprites/global/criteria/mobs/vindicator.png", "vindicator");
    loadCriteria("assets/sprites/global/criteria/mobs/witch.png", "witch");
    loadCriteria("assets/sprites/global/criteria/mobs/wither^16.png", "wither^16");
    loadCriteria("assets/sprites/global/criteria/mobs/wither^32.png", "wither^32");
    loadCriteria("assets/sprites/global/criteria/mobs/wither^48.png", "wither");
    loadCriteria("assets/sprites/global/criteria/mobs/wither_skeleton.png", "wither_skeleton");
    loadCriteria("assets/sprites/global/criteria/mobs/zoglin.png", "zoglin");
    loadCriteria("assets/sprites/global/criteria/mobs/zombie_pigman.png", "zombie_pigman");
    loadCriteria("assets/sprites/global/criteria/mobs/zombie.png", "zombie");
    loadCriteria("assets/sprites/global/criteria/mobs/zombie_villager.png", "zombie_villager");
    loadCriteria("assets/sprites/global/criteria/mobs/zombified_piglin.png", "zombified_piglin");
    current_ = nullptr;
}
