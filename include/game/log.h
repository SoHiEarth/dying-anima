#ifndef GAME_LOG_H
#define GAME_LOG_H
#include "game/degradation.h"
#include <string>
#include <map>
#include <vector>

namespace game {
  struct LogEntry {
    std::map<DegradationLevel, std::string> title;
    std::map<DegradationLevel, std::string> description;
    std::string timestamp;
    std::shared_ptr<Texture> texture;
  };
  struct Log {
    void NewLog();
    void LoadLog();
    void SaveLog();
    std::vector<LogEntry> GetLogs() { return log; }
  private:
    std::vector<LogEntry> log;
  };
}

#endif