#include "game/enemy.h"

#include <iostream>
#include <pugixml.hpp>

#include "core/scene.h"
#include "core/transform.h"
#include "game/battle.h"
#include "game/player.h"

namespace {
std::vector<Enemy> default_enemies;
std::once_flag default_enemies_initialized;

void LoadEnemies() {
  pugi::xml_document doc;
  if (!doc.load_file("assets/enemies.xml")) {
    std::print("Failed to load enemies file: assets/enemies.xml\n");
    return;
  }
  auto root = doc.child("Enemies");
  for (auto object_node : root.children("Enemy")) {
    Enemy enemy;
    enemy.name = object_node.attribute("name").as_string();
    enemy.max_health = object_node.attribute("max_health").as_float();
    enemy.health = enemy.max_health;
    enemy.max_stamina = object_node.attribute("max_stamina").as_float();
    enemy.stamina = enemy.max_stamina;
    enemy.damage = object_node.attribute("damage").as_float();
    default_enemies.push_back(enemy);
  }
  std::cout << "Loaded " << default_enemies.size() << " enemy presets\n";
}

}  // namespace

Enemy game::CreateEnemyFromName(std::string_view name) {
  // load default enemies if not already loaded
  if (default_enemies.empty()) {
    std::call_once(default_enemies_initialized, LoadEnemies);
  }

  for (const auto& enemy : default_enemies) {
    std::cout << "Found enemy preset: " << enemy.name << "\n";
    if (enemy.name == name) {
      return enemy;
    }
  }
  throw std::runtime_error("Enemy type not found: " + std::string(name));
}

void game::UpdateBattleTriggers(entt::registry& registry,
                                SceneManager& scene_manager) {
  auto view = registry.view<BattleTrigger>();
  for (auto entity : view) {
    auto& damager = view.get<BattleTrigger>(entity);
    auto& transform = registry.get<Transform>(entity);
    auto health_view = registry.view<Health>();
    for (auto health_entity : health_view) {
      auto& player_transform = registry.get<Transform>(health_entity);
      float distance =
          glm::distance(transform.position, player_transform.position);
      if (distance < damager.hitbox_radius) {
        scene_manager.PopScene();
        scene_manager.PushScene(
            std::make_unique<BattleScene>(scene_manager, damager.enemies));
      }
    }
  }
}
