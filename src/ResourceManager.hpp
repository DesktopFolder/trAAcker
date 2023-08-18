#pragma once

#include <SFML/Graphics/Texture.hpp>
#include <string>
#include <unordered_map>

namespace aa
{
struct ResourceManager
{
    static ResourceManager& instance();
    static std::unordered_map<std::string, std::string> getAllAssets();
    static std::string assetName(std::string filePath) {
        if (filePath.size() < 3) return filePath;
        // remove .png
        if (filePath.ends_with(".png")) filePath = filePath.substr(0, filePath.size() - 4);
        if (filePath[filePath.size() - 3] == '^') return assetName(std::string{filePath.begin(), filePath.end() - 3});

        auto it = std::find(filePath.crbegin(), filePath.crend(), '/');
        if (it == filePath.crend()) return filePath;
        return std::string{it.base(), filePath.cend()};
    }
    void loadAllCriteria();

    void ensureLoadAdvancement(std::string id, std::string icon);

    std::unordered_map<std::string, std::unordered_map<std::string, sf::Texture>> criteria;
private:
    ResourceManager() { loadAllCriteria(); }
    void loadCriteria(std::string, std::string);
    // IMPLEMENTATION DETAIL DON'T LOOK IT IS VERY BAD :)
    std::unordered_map<std::string, sf::Texture>* current_ = nullptr;
};
}  // namespace aa
