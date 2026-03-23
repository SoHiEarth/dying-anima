#ifndef GAME_LOG_H
#define GAME_LOG_H
#include <map>
#include <string>
#include <vector>

#include "core/font.h"
#include "core/resource_manager.h"
#include "core/texture.h"
#include "game/degradation.h"

namespace game {
struct LogEntry {
  std::map<DegradationLevel, std::string> title;
  std::map<DegradationLevel, std::string> description;
  std::string timestamp;
  TextureHandle texture;
};
// Consider moving Load/Save to SaveData and let savedata handle operations
struct Log {
  void NewLog(const LogEntry& entry);
  void LoadLog();
  void SaveLog();
  std::vector<LogEntry> GetLogs() { return log_; }
  // Check if a log w/ a title at the level exists.
  bool LogExists(const std::string& title, DegradationLevel level);
  void RenderLog(DegradationLevel degrade_level);

 private:
  std::vector<LogEntry> log_;
};
}  // namespace game

#endif
