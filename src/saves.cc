#include "saves.h"

#include <chrono>
#include <filesystem>
#include <pugixml.hpp>

#include "core/log.h"
#include "game/log.h"

namespace {
std::string savedata_root_name = "SaveData";
std::string savedata_directory = "saves";
}

void save_manager::SaveGame(const SaveData& data, const game::Log& log) {
  pugi::xml_document doc;
  auto root = doc.append_child(savedata_root_name);
  auto player_node = root.append_child("Player");
  player_node.append_attribute("death_count") = data.death_count;
  player_node.append_attribute("pos.x") = data.player_transform.position.x;
  player_node.append_attribute("pos.y") = data.player_transform.position.y;
  auto health_node = player_node.append_child("Health");
  health_node.append_attribute("max_health") = data.player_health.max_health;
  health_node.append_attribute("max_stamina") = data.player_health.max_stamina;
  health_node.append_attribute("health") = data.player_health.health;
  health_node.append_attribute("stamina") = data.player_health.stamina;

  log.SaveLog(root);
  auto comp_marker_node = root.append_child("Completion");
  for (const auto& marker : data.completion_markers) {
    auto marker_node = comp_marker_node.append_child("Marker");
    marker_node.append_attribute("name") = marker.c_str();
  }

  auto skills_node = root.append_child("Skills");
  for (const auto& skill : data.acquired_skills) {
    auto skill_node = skills_node.append_child("Skill");
    skill_node.append_attribute("name") = skill.name;
    skill_node.append_attribute("damage") = skill.damage;
    skill_node.append_attribute("health_used") = skill.health_used;
    skill_node.append_attribute("stamina_used") = skill.stamina_used;
  }

  auto defeated_enemies_node = root.append_child("DefeatedEnemies");
  for (const auto& uid : data.defeated_enemy_uids) {
    auto enemy_node = defeated_enemies_node.append_child("Enemy");
    enemy_node.append_attribute("uid") = uid;
  }

  if (!std::filesystem::exists(savedata_directory)) {
    std::filesystem::create_directory(savedata_directory);
  }
  auto now = std::chrono::system_clock::now();
  auto t = std::chrono::system_clock::to_time_t(now);
  auto tm = *std::localtime(&t);
  std::stringstream ss;
  ss << std::put_time(&tm, "%Y-%m-%d_%H-%M-%S");

  std::string filename =
      (std::filesystem::path(savedata_directory) /
       std::filesystem::path(ss.str() + ".save")).string();
  core::Log(std::format("Saving game to {}", filename), "Saves");
  doc.save_file(filename.c_str());
}

std::expected<SaveData, LoadError> save_manager::LoadGame(std::string_view filename) {
  if (!std::filesystem::exists(filename))
    return std::unexpected(LoadError::kFileNotFound);
  
  SaveData data{};
  pugi::xml_document doc;
  if (!doc.load_file(std::string(filename).c_str())) {
    throw core::Error("Failed to load save file", "SaveManager");
  }
  auto root_node = doc.child(savedata_root_name);
  auto player_node = root_node.child("Player");
  data.death_count = player_node.attribute("death_count").as_int();
  if (player_node) {
    data.player_transform.position.x =
        player_node.attribute("pos.x").as_float();
    data.player_transform.position.y =
        player_node.attribute("pos.y").as_float();
    auto health_node = player_node.child("Health");
    data.player_health.max_health = health_node.attribute("max_health").as_float();
    data.player_health.max_stamina = health_node.attribute("max_stamina").as_float();
    data.player_health.health = health_node.attribute("health").as_float();
    data.player_health.stamina = health_node.attribute("stamina").as_float();
  }
  data.log.LoadLog(root_node);
  auto comp_marker_node = root_node.child("Completion");
  for (auto marker : comp_marker_node.children("Marker")) {
    data.completion_markers.emplace_back(marker.attribute("name").as_string());
  }

  auto skills_node = root_node.child("Skills");
  for (auto skill_node : skills_node.children("Skill")) {
    Skill skill;
    skill.name = skill_node.attribute("name").as_string();
    skill.damage = skill_node.attribute("damage").as_float();
    skill.health_used = skill_node.attribute("health_used").as_float();
    skill.stamina_used = skill_node.attribute("stamina_used").as_float();
    data.acquired_skills.emplace_back(skill);
  }

  auto defeated_enemies_node = root_node.child("DefeatedEnemies");
  for (auto enemy_node : defeated_enemies_node.children("Enemy")) {
    data.defeated_enemy_uids.emplace_back(enemy_node.attribute("uid").as_int());
  }
  core::Log(std::format("Loaded game from {}", filename), "SaveManager");
  data.valid = true;
  return data;
}

std::expected<SaveData, LoadError> save_manager::LoadLatestSave() {
  if (!std::filesystem::exists("saves")) {
    core::Log("Saves directory doesn't exist. Recreating directory.", "SaveManager");
    std::filesystem::create_directory("saves");
    return {}; // No need to search empty directory
  }
  
  std::string latest_save;
  std::filesystem::file_time_type latest_time{};

  for (const auto& entry :
       std::filesystem::directory_iterator(savedata_directory)) {
    if (entry.is_regular_file() && entry.path().extension() == ".save") {
      std::string filename = entry.path().filename().string();

      auto file_time = entry.last_write_time();
      if (file_time > latest_time) {
        latest_time = file_time;
        latest_save = filename;
      }
    }
  }

  return LoadGame("saves/" + latest_save);
}
