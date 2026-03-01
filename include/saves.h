#pragma once
#include "player.h"
#include "transform.h"
#include <string>

struct SaveData {
  bool valid = false;
  Transform player_transform;
  Health player_health;
  std::vector<std::string> completion_markers;
};

namespace SaveManager {
void SaveGame(const SaveData& data);
SaveData LoadGame(std::string_view file_name);
SaveData LoadLatestSave();
} // namespace SaveManager
