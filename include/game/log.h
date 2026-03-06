#ifndef GAME_LOG_H
#define GAME_LOG_H
#include "game/degradation.h"
#include <string>
#include <map>
#include <vector>
#include "core/texture.h"

namespace game {
  struct LogEntry {
    std::map<DegradationLevel, std::string> title;
    std::map<DegradationLevel, std::string> description;
    std::string timestamp;
    std::shared_ptr<Texture> texture;
  };
  // Consider moving Load/Save to SaveData and let savedata handle operations
  struct Log {
    void NewLog(const LogEntry& entry);
    void LoadLog();
    void SaveLog();
    std::vector<LogEntry> GetLogs() { return log; }
  private:
    std::vector<LogEntry> log;
  };
}

#endif