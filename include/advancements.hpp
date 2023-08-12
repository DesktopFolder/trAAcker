#pragma once

#include <string>
#include <optional>

struct Advancement {
    std::optional<std::string> id;
    std::optional<std::string> name;
    std::optional<std::string> icon;
    std::optional<std::string> type;
    std::optional<std::string> short_name;
};
constexpr Advancement root{ "minecraft:adventure/root", "Adventure", "adventure", {}, {}, };
constexpr Advancement kill_a_mob{ "minecraft:adventure/kill_a_mob", "Monster Hunter", {}, {}, "Hunter", };
constexpr Advancement ol_betsy{ "minecraft:adventure/ol_betsy", "Ol' Betsy", {}, {}, {}, };
constexpr Advancement whos_the_pillager_now{ "minecraft:adventure/whos_the_pillager_now", "Who's the Pillager Now?", {}, {}, "The Pillager", };
constexpr Advancement sleep_in_bed{ "minecraft:adventure/sleep_in_bed", "Sweet Dreams", {}, {}, "Dreams", };
constexpr Advancement trade{ "minecraft:adventure/trade", "What a Deal!", {}, {}, "What a Deal", };
constexpr Advancement two_birds_one_arrow{ "minecraft:adventure/two_birds_one_arrow", "Two Birds, One Arrow", {}, "challenge", "Two Birds", };
constexpr Advancement arbalistic{ "minecraft:adventure/arbalistic", "Arbalistic", {}, "challenge", {}, };
constexpr Advancement honey_block_slide{ "minecraft:adventure/honey_block_slide", "Sticky Situation", {}, {}, "Sticky", };
constexpr Advancement bullseye{ "minecraft:adventure/bullseye", "Bullseye", {}, "challenge", {}, };
constexpr Advancement shoot_arrow{ "minecraft:adventure/shoot_arrow", "Take Aim", {}, {}, {}, };
constexpr Advancement sniper_duel{ "minecraft:adventure/sniper_duel", "Sniper Duel", {}, "challenge", {}, };
constexpr Advancement voluntary_exile{ "minecraft:adventure/voluntary_exile", "Voluntary Exile", "pillager_banner", {}, "Exile", };
constexpr Advancement hero_of_the_village{ "minecraft:adventure/hero_of_the_village", "Hero of the Village", "pillager_banner", "challenge", "Hero OTV", };
constexpr Advancement throw_trident{ "minecraft:adventure/throw_trident", "A Throwaway Joke", {}, {}, "ATJ", };
constexpr Advancement very_very_frightening{ "minecraft:adventure/very_very_frightening", "Very Very Frightening", {}, {}, "VVF", };
constexpr Advancement summon_iron_golem{ "minecraft:adventure/summon_iron_golem", "Hired Help", {}, "goal", {}, };
constexpr Advancement totem_of_undying{ "minecraft:adventure/totem_of_undying", "Postmortal", {}, "goal", {}, };
constexpr Advancement adventuring_time{ "minecraft:adventure/adventuring_time", "Adventuring Time", {}, "challenge", "Adventuring", };
constexpr Advancement kill_all_mobs{ "minecraft:adventure/kill_all_mobs", "Monsters Hunted", {}, "challenge", "Monsters", };
constexpr Advancement root{ "minecraft:end/root", "The End", "end", {}, {}, };
constexpr Advancement kill_dragon{ "minecraft:end/kill_dragon", "&#160;Free&#160;the&#160; End", {}, {}, "Free End", };
constexpr Advancement dragon_egg{ "minecraft:end/dragon_egg", "The Next Generation", {}, "goal", "Next Gen", };
constexpr Advancement enter_end_gateway{ "minecraft:end/enter_end_gateway", "Remote Getaway", {}, {}, "Getaway", };
constexpr Advancement find_end_city{ "minecraft:end/find_end_city", "City at the End of the Game", {}, {}, "End City", };
constexpr Advancement dragon_breath{ "minecraft:end/dragon_breath", "You Need a&#160;Mint", {}, "goal", "Need a Mint", };
constexpr Advancement respawn_dragon{ "minecraft:end/respawn_dragon", "The End... Again...", {}, "goal", "End Again", };
constexpr Advancement elytra{ "minecraft:end/elytra", "Sky's the Limit", {}, "goal", "The Limit", };
constexpr Advancement levitate{ "minecraft:end/levitate", "Great View From Here", {}, "challenge", "Great View", };
constexpr Advancement root{ "minecraft:husbandry/root", "Husbandry", "husbandry", {}, {}, };
constexpr Advancement plant_seed{ "minecraft:husbandry/plant_seed", "A Seedy Place", {}, {}, "Seedy Place", };
constexpr Advancement breed_an_animal{ "minecraft:husbandry/breed_an_animal", "The Parrots and the Bats", {}, {}, "Parrots", };
constexpr Advancement tame_an_animal{ "minecraft:husbandry/tame_an_animal", "Best Friends Forever", {}, {}, "BFF's", };
constexpr Advancement fishy_business{ "minecraft:husbandry/fishy_business", "Fishy Business", {}, {}, "Fishy Biz", };
constexpr Advancement tactical_fishing{ "minecraft:husbandry/tactical_fishing", "Tactical Fishing", {}, {}, "Tactical", };
constexpr Advancement silk_touch_nest{ "minecraft:husbandry/silk_touch_nest", "Total Beelocation", {}, {}, "Beelocation", };
constexpr Advancement safely_harvest_honey{ "minecraft:husbandry/safely_harvest_honey", "Bee Our Guest", {}, {}, "Our Guest", };
constexpr Advancement obtain_netherite_hoe{ "minecraft:husbandry/obtain_netherite_hoe", "Serious Dedication", {}, "challenge", "Dedication", };
constexpr Advancement bred_all_animals{ "minecraft:husbandry/bred_all_animals", "Two by Two", {}, "challenge", {}, };
constexpr Advancement balanced_diet{ "minecraft:husbandry/balanced_diet", "A Balanced Diet", {}, "challenge", "ABD", };
constexpr Advancement complete_catalogue{ "minecraft:husbandry/complete_catalogue", "A Complete Catalogue", {}, "challenge", "Catalogue", };
constexpr Advancement return_to_sender{ "minecraft:nether/return_to_sender", "Return to Sender", {}, "challenge", "Sender", };
constexpr Advancement use_lodestone{ "minecraft:nether/use_lodestone", "Country&#160;Lode, Take Me Home", {}, {}, "Lodestone", };
constexpr Advancement charge_respawn_anchor{ "minecraft:nether/charge_respawn_anchor", "Not Quite "Nine" Lives", {}, {}, "Nine Lives", };
constexpr Advancement obtain_crying_obsidian{ "minecraft:nether/obtain_crying_obsidian", "Who's Cutting Onions?", {}, {}, "Onions?", };
constexpr Advancement loot_bastion{ "minecraft:nether/loot_bastion", "War Pigs", {}, {}, {}, };
constexpr Advancement find_bastion{ "minecraft:nether/find_bastion", "Those Were the Days", {}, {}, "The Days", };
constexpr Advancement find_fortress{ "minecraft:nether/find_fortress", "A Terrible Fortress", {}, {}, "Fortress", };
constexpr Advancement root{ "minecraft:nether/root", "Nether", "nether", {}, {}, };
constexpr Advancement uneasy_alliance{ "minecraft:nether/uneasy_alliance", "Uneasy Alliance", {}, "challenge", "Uneasy", };
constexpr Advancement fast_travel{ "minecraft:nether/fast_travel", "Subspace Bubble", {}, "challenge", "Subspace", };
constexpr Advancement brew_potion{ "minecraft:nether/brew_potion", "Local Brewery", {}, {}, "Brewery", };
constexpr Advancement obtain_blaze_rod{ "minecraft:nether/obtain_blaze_rod", "Into Fire", {}, {}, {}, };
constexpr Advancement distract_piglin{ "minecraft:nether/distract_piglin", "Oh Shiny", {}, {}, {}, };
constexpr Advancement obtain_ancient_debris{ "minecraft:nether/obtain_ancient_debris", "Hidden in the&#160;Depths", {}, {}, "The Depths", };
constexpr Advancement explore_nether{ "minecraft:nether/explore_nether", "Hot Tourist Destinations", {}, "challenge", "Hot Tourist", };
constexpr Advancement netherite_armor{ "minecraft:nether/netherite_armor", "Cover Me in&#160;Debris", {}, "challenge", "In Debris", };
constexpr Advancement all_effects{ "minecraft:nether/all_effects", "How Did We Get Here?", {}, "challenge", "HDWGH", };
constexpr Advancement all_potions{ "minecraft:nether/all_potions", "A Furious Cocktail", {}, "challenge", "Cocktail", };
constexpr Advancement create_full_beacon{ "minecraft:nether/create_full_beacon", "Beaconator", "beacon", "goal", "Beaconator", };
constexpr Advancement create_beacon{ "minecraft:nether/create_beacon", "Bring Home the Beacon", "beacon", {}, "Beacon", };
constexpr Advancement summon_wither{ "minecraft:nether/summon_wither", "Withering Heights", {}, {}, "Withering", };
constexpr Advancement get_wither_skull{ "minecraft:nether/get_wither_skull", "Spooky&#160;Scary Skeleton", {}, {}, "Spooky", };
constexpr Advancement ride_strider{ "minecraft:nether/ride_strider", "This Boat Has Legs", {}, {}, "This Boat", };
constexpr Advancement mine_stone{ "minecraft:story/mine_stone", "Stone Age", {}, {}, {}, };
constexpr Advancement upgrade_tools{ "minecraft:story/upgrade_tools", "Getting an Upgrade", {}, {}, "Upgrade", };
constexpr Advancement iron_tools{ "minecraft:story/iron_tools", "Isn't It Iron&#160;Pick", {}, {}, "Iron Pick", };
constexpr Advancement smelt_iron{ "minecraft:story/smelt_iron", "Acquire Hardware", {}, {}, "Hardware", };
constexpr Advancement mine_diamond{ "minecraft:story/mine_diamond", "Diamonds!", {}, {}, {}, };
constexpr Advancement lava_bucket{ "minecraft:story/lava_bucket", "Hot Stuff", {}, {}, {}, };
constexpr Advancement form_obsidian{ "minecraft:story/form_obsidian", "Ice Bucket Challenge", {}, {}, "Ice Bucket", };
constexpr Advancement enter_the_nether{ "minecraft:story/enter_the_nether", "We Need to Go Deeper", {}, {}, "Go Deeper", };
constexpr Advancement obtain_armor{ "minecraft:story/obtain_armor", "Suit Up", {}, {}, {}, };
constexpr Advancement shiny_gear{ "minecraft:story/shiny_gear", "Cover Me With Diamonds", {}, {}, "W/Diamonds", };
constexpr Advancement cure_zombie_villager{ "minecraft:story/cure_zombie_villager", "Zombie Doctor", {}, "goal", "Zombie Doc", };
constexpr Advancement root{ "minecraft:story/root", "Minecraft", "grass", {}, {}, };
constexpr Advancement deflect_arrow{ "minecraft:story/deflect_arrow", "Not Today, Thank You", {}, {}, "Not Today", };
constexpr Advancement follow_ender_eye{ "minecraft:story/follow_ender_eye", "Eye Spy", {}, {}, {}, };
constexpr Advancement enter_the_end{ "minecraft:story/enter_the_end", "The End?", "end", {}, {}, };
constexpr Advancement enchant_item{ "minecraft:story/enchant_item", "Enchanter", {}, {}, {}, };
