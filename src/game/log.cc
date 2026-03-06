#include "game/log.h"
#include <chrono>
#include <pugixml.hpp>
#define LOG_FILE "player_log.xml"

void game::Log::NewLog(const LogEntry& entry) {
  auto now = std::chrono::system_clock::now();
  auto now_c = std::chrono::system_clock::to_time_t(now);
  LogEntry new_entry = entry;
  new_entry.timestamp = std::ctime(&now_c);
  log.push_back(new_entry);
}

void game::Log::LoadLog() { 
  auto doc = pugi::xml_document();
  doc.load_file(LOG_FILE);
  auto entries = doc.child("logs").children("entry");
  for (auto& entry : entries) {
    LogEntry log_entry;
    log_entry.timestamp = entry.child("timestamp").text().as_string();
    for (auto& title : entry.child("title").children("degradation")) {
      auto level = static_cast<DegradationLevel>(title.attribute("level").as_int());
      log_entry.title[level] = title.text().as_string();
    }
    for (auto& description : entry.child("description").children("degradation")) {
      auto level = static_cast<DegradationLevel>(description.attribute("level").as_int());
      log_entry.description[level] = description.text().as_string();
    }
    log.push_back(log_entry);
  }
}

void game::Log::SaveLog() {
  auto doc = pugi::xml_document();
  auto logs_node = doc.append_child("logs");
  for (const auto& entry : log) {
    auto entry_node = logs_node.append_child("entry");
    entry_node.append_child("timestamp").text().set(entry.timestamp.c_str());
    auto title_node = entry_node.append_child("title");
    for (const auto& title : entry.title) {
      auto degradation_node = title_node.append_child("degradation");
      degradation_node.append_attribute("level") = static_cast<int>(title.first);
      degradation_node.text().set(title.second.c_str());
    }
    auto description_node = entry_node.append_child("description");
    for (const auto& description : entry.description) {
      auto degradation_node = description_node.append_child("degradation");
      degradation_node.append_attribute("level") = static_cast<int>(description.first);
      degradation_node.text().set(description.second.c_str());
    }
  }
  doc.save_file(LOG_FILE);
}