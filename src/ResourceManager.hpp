#pragma once

#include <SFML/Graphics/Texture.hpp>
#include <string>
#include <unordered_map>

namespace aa
{
struct ResourceManager
{
    static ResourceManager& instance();
    void loadAllCriteria();

    std::unordered_map<std::string, std::unordered_map<std::string, sf::Texture>> criteria;
private:
    ResourceManager() { loadAllCriteria(); }
    void loadCriteria(std::string, std::string);
    // IMPLEMENTATION DETAIL DON'T LOOK IT IS VERY BAD :)
    std::unordered_map<std::string, sf::Texture>* current_ = nullptr;
};
}  // namespace aa
