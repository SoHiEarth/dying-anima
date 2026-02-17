#pragma once
#include "player.h"

struct SaveData {
  std::string file_name;
  Player player;
};

namespace SaveManager {
void SaveGame(const Player &player);
SaveData LoadGame(std::string_view file_name);
} // namespace SaveManager
