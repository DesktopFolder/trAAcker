#include "Map.hpp"
#include "ConfigProvider.hpp"
#include "logging.hpp"

#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace aa
{
MapManager::MapManager(const nlohmann::json& config)
{
}

void MapManager::debug()
{
    auto& logger = get_logger("MapManager::debug");
}
} // namespace aa