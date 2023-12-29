#pragma once

#include <SFML/Graphics/Texture.hpp>
#include <string>
#include <unordered_map>
#include <memory>

namespace aa
{
struct ResourceManager
{
    static ResourceManager& instance();
    static std::unordered_map<std::string, std::string> getAllAssets();
    static std::string assetName(std::string filePath) {
        if (filePath.size() < 3) return filePath;
        // remove .png
        if (filePath.ends_with(".png") || filePath.ends_with(".gif")) filePath = filePath.substr(0, filePath.size() - 4);
        if (filePath[filePath.size() - 3] == '^') return assetName(std::string{filePath.begin(), filePath.end() - 3});

        auto it = std::find(filePath.crbegin(), filePath.crend(), '/');
        if (it == filePath.crend()) return filePath;
        return std::string{it.base(), filePath.cend()};
    }
    void loadAllCriteria();

    void commit() { no_load = true; }

    void ensureLoadAdvancement(std::string id, std::string icon);

    sf::Texture* store_texture_at(std::string path) {
        random_textures.emplace_back(std::make_unique<sf::Texture>());
        random_textures.back()->loadFromFile(path);
        return random_textures.back().get();
    }

    std::vector<std::unique_ptr<sf::Texture>> random_textures;

    // unused atm
    std::unordered_map<std::string, std::unordered_map<std::string, sf::Texture>> criteria;
    // use this for now
    std::unordered_map<std::string, sf::Texture> test_criteria;
private:
    ResourceManager() { loadAllCriteria(); }
    void loadCriteria(std::string, std::string);
    // IMPLEMENTATION DETAIL DON'T LOOK IT IS VERY BAD :)
    // later: wtf
    std::unordered_map<std::string, sf::Texture>* current_ = nullptr;
    bool no_load = false;
};
}  // namespace aa
