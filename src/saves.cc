#include "saves.h"
#include <chrono>
#include <filesystem>
#include <iostream>
#include <pugixml.hpp>

void SaveManager::SaveGame(const Transform &player_transform,
                           const Health &player_health) {
  pugi::xml_document doc;
  auto root = doc.append_child("SaveData");
  auto player_node = root.append_child("Player");
  player_node.append_attribute("pos.x") = player_transform.position.x;
  player_node.append_attribute("pos.y") = player_transform.position.y;
  player_node.append_attribute("Health") = player_health.health;
  if (!std::filesystem::exists("saves")) {
    std::filesystem::create_directory("saves");
  }
  std::string file_name =
      (std::filesystem::path("saves") /
       std::filesystem::path(
           std::to_string(
               std::chrono::system_clock::now().time_since_epoch().count()) +
           ".save"))
          .string();
  std::cout << "Saving game to " << file_name << std::endl;
  doc.save_file(file_name.c_str());
}

SaveData SaveManager::LoadGame(std::string_view file_name) {
  SaveData data{};
  data.file_name = std::string(file_name);
  pugi::xml_document doc;
  if (!doc.load_file(file_name.data())) {
    throw std::runtime_error("Failed to load save file");
  }
  auto player_node = doc.child("SaveData").child("Player");
  if (player_node) {
    data.player_transform.position.x =
        player_node.attribute("pos.x").as_float();
    data.player_transform.position.y =
        player_node.attribute("pos.y").as_float();
    data.player_health.health = player_node.attribute("Health").as_float();
  }
  std::cout << "Loaded game from " << file_name << std::endl;
  return data;
}

SaveData SaveManager::LoadLatestSave() {
  std::string largest_save_id = "";
  for (const auto &entry : std::filesystem::directory_iterator("saves")) {
    if (entry.is_regular_file() && entry.path().extension() == ".save") {
      std::string filename = entry.path().filename().string();
      std::string save_id = filename.substr(0, filename.find(".save"));
      largest_save_id = std::max(largest_save_id, save_id);
    }
  }
  return LoadGame("saves/" + largest_save_id + ".save");
}
