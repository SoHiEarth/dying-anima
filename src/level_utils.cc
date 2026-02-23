#include "level_utils.h"

#include "physics.h"
#include "sprite.h"
#include "transform.h"
#include "core/resource_manager.h"
#include <fstream>
#include <print>
#include <sstream>

std::vector<Object> LoadLevel(std::string_view filename,
                              entt::registry &registry) {
  std::vector<Object> objects{};
  std::fstream file(filename.data());
  if (!file.is_open()) {
    std::print("Failed to open level file: {}\n", filename);
    return {};
  }
  std::string line;
  while (std::getline(file, line)) {
    std::istringstream iss(line);
    auto entity = registry.create();
    auto &transform = registry.emplace<Transform>(entity);
    if (!(iss >> transform.position.x >> transform.position.y)) {
      std::print("Error reading position in level file: {}\n", line);
      continue;
    }
    if (!(iss >> transform.scale.x >> transform.scale.y)) {
      std::print("Error reading scale in level file: {}\n", line);
      continue;
    }
    if (!(iss >> transform.rotation)) {
      std::print("Error reading rotation in level file: {}\n", line);
      continue;
    }
    auto &sprite = registry.emplace<Sprite>(entity);
    if (!(iss >> sprite.texture_tag)) {
      std::print("Error reading texture tag in level file: {}\n", line);
      continue;
    }
    sprite.texture = ResourceManager::GetTexture(sprite.texture_tag).texture;
    registry
                    .emplace<PhysicsBody>(
                        entity, physics::CreateBody(transform.position,
                                    transform.scale, transform.rotation, true))
                    .body;
  }
  file.close();
  return objects;
}

void SaveLevel(std::string_view filename, const std::vector<Object> &objects) {
  std::ofstream file(filename.data());
  if (!file.is_open()) {
    std::print("Failed to open level file for writing: {}\n", filename);
    return;
  }
  for (const auto &obj : objects) {
    file << obj.position.x << " " << obj.position.y << " ";
    file << obj.scale.x << " " << obj.scale.y << " ";
    file << obj.rotation << " ";
    file << "util.notexture" << "\n";
  }
  file.close();
}
