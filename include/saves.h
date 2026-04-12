#ifndef SAVES_H
#define SAVES_H

#include <expected>
#include <string>

#include "core/transform.h"
#include "game/log.h"
#include "game/player.h"

struct SaveData {
  bool valid = false;
  Transform player_transform;
  Health player_health;
  std::vector<std::string> completion_markers;
  int death_count = 0;
  game::Log log;
  std::vector<Skill> acquired_skills;
  std::vector<int> defeated_enemy_uids;
};

enum class LoadError { kFileNotFound };

namespace save_manager {
void SaveGame(const SaveData& data, const game::Log& log);
std::expected<SaveData, LoadError> LoadGame(std::string_view filename);
std::expected<SaveData, LoadError> LoadLatestSave();
}  // namespace save_manager

#endif  // SAVES_H
