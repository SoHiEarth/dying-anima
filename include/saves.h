#pragma once
#include "player.h"
#include "transform.h"
#include <string>

struct SaveData {
  std::string file_name;
  Transform player_transform;
  Health player_health;
};

namespace SaveManager {
void SaveGame(const Transform &player, const Health &player_health);
SaveData LoadGame(std::string_view file_name);
SaveData LoadLatestSave();
} // namespace SaveManager
