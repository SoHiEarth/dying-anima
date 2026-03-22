#include "game/log.h"

#include <chrono>
#include <iomanip>
#include <pugixml.hpp>
#include <sstream>
#include "core/resource_manager.h"
#include "util/colors.h"
#define LOG_FILE "player_log.xml"
#include "core/window.h"
#include "util/colors.h"
#include <imgui.h>

constexpr float kLogMargin = 50.0f;

void game::Log::NewLog(const LogEntry& entry) {
  auto now = std::chrono::system_clock::now();
  auto now_c = std::chrono::system_clock::to_time_t(now);
  LogEntry new_entry = entry;

  std::tm tm_buf;
#ifdef _WIN32
  localtime_s(&tm_buf, &now_c);
#else
  localtime_r(&now_c, &tm_buf);
#endif

  std::ostringstream oss;
  oss << std::put_time(&tm_buf, "%c");
  new_entry.timestamp = oss.str();

  log.push_back(new_entry);
}

void game::Log::LoadLog() {
  log.clear();
  auto doc = pugi::xml_document();
  doc.load_file(LOG_FILE);
  auto entries = doc.child("logs").children("entry");
  for (auto& entry : entries) {
    LogEntry log_entry;
    log_entry.timestamp = entry.child("timestamp").text().as_string();
    for (auto& title : entry.child("title").children("degradation")) {
      auto level =
          static_cast<DegradationLevel>(title.attribute("level").as_int());
      log_entry.title[level] = title.text().as_string();
    }
    for (auto& description :
         entry.child("description").children("degradation")) {
      auto level = static_cast<DegradationLevel>(
          description.attribute("level").as_int());
      log_entry.description[level] = description.text().as_string();
    }
    log_entry.texture = ResourceManager::GetTexture(
        entry.child("texture").attribute("tag").as_string());
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
    for (const auto& [level, title] : entry.title) {
      auto degradation_node = title_node.append_child("degradation");
      degradation_node.append_attribute("level") =
          static_cast<int>(level);
      degradation_node.text().set(title.c_str());
    }
    auto description_node = entry_node.append_child("description");
    for (const auto& [level, description] : entry.description) {
      auto degradation_node = description_node.append_child("degradation");
      degradation_node.append_attribute("level") =
          static_cast<int>(level);
      degradation_node.text().set(description.c_str());
    }
    auto texture_node = entry_node.append_child("texture");
    texture_node.append_attribute("tag") = entry.texture.tag.c_str();
  }
  doc.save_file(LOG_FILE);
}

int selected_log_index = -1;

void game::Log::RenderLog(DegradationLevel degrade_level) {
  ImGui::SetNextWindowPos(ImVec2(kLogMargin, kLogMargin), ImGuiCond_Always);
  auto size = ImVec2(GetGameWindow().width - kLogMargin * 2, GetGameWindow().height - kLogMargin * 2);
  ImGui::SetNextWindowSize(size, ImGuiCond_Always); 
  ImGui::Begin("PlayerLog", nullptr,
               ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
                   ImGuiWindowFlags_NoResize);
  ImGui::Text("Journal");
  if (ImGui::BeginChild("EntryList", ImVec2(ImGui::GetContentRegionAvail().x / 3.0f, 0))) {
    ImGui::Text("All Entries");
    int i = 0;
    for (const auto& entry : log) {
      for (const auto& title : entry.title) {
        if (ImGui::Selectable(std::format("{}##{}", title.second, i).c_str())) {
          selected_log_index = i;
        }
      }
      i++;
    }
    ImGui::EndChild();
  }
  ImGui::SameLine();
  if (selected_log_index >= 0 && selected_log_index < log.size()) {
    if (ImGui::BeginChild("EntryDetails", ImVec2(0, 0))) {
      auto entry = log.at(selected_log_index);
      ImGui::Text("Details");
      ImGui::Image(entry.texture.texture ? entry.texture.texture->id : 0,
                   ImVec2(ImGui::GetWindowSize().x, 200),
                   IMGUI_TEXTURE_FLIP);
      ImGui::Separator();
      ImGui::Text(entry.description.at(degrade_level).c_str());
      ImGui::EndChild();
    }
  }
  ImGui::End();
}