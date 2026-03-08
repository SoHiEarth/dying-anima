#pragma once
#include <string>

#include "core/transform.h"
#include "game/player.h"

struct SaveData {
  bool valid = false;
  Transform player_transform;
  Health player_health;
  std::vector<std::string> completion_markers;
};

namespace SaveManager {
void SaveGame(const SaveData& data);
SaveData LoadGame(std::string_view filename);
SaveData LoadLatestSave();
}  // namespace SaveManager
