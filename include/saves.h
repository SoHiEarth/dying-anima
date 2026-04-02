#ifndef SAVES_H
#define SAVES_H

#include <string>

#include "core/transform.h"
#include "game/log.h"
#include "game/player.h"

struct SaveData {
  bool valid = false;
  Transform player_transform;
  Health player_health;
  std::vector<std::string> completion_markers;
  game::Log log;
  std::vector<Skill> acquired_skills;
};

namespace save_manager {
void SaveGame(const SaveData& data, const game::Log& log);
SaveData LoadGame(std::string_view filename);
SaveData LoadLatestSave();
}  // namespace save_manager

#endif  // SAVES_H
