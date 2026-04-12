#include "game/enemy.h"

#include <algorithm>
#include <mutex>
#include <pugixml.hpp>

#include "core/log.h"
#include "core/path_resolve.h"
#include "core/resource_manager.h"
#include "core/scene.h"
#include "core/transform.h"
#include "game/battle.h"
#include "game/player.h"

namespace {
std::vector<Enemy> default_enemies;
std::once_flag default_enemies_initialized;

void LoadEnemies() {
  pugi::xml_document doc;
  if (!doc.load_file((core::path::GetAssetPath() / "enemies.xml").c_str())) {
    throw core::Error(std::format("Failed to load enemies file"), "Enemy");
  }
  auto root = doc.child("Enemies");
  for (auto object_node : root.children("Enemy")) {
    Enemy enemy;
    enemy.name = object_node.attribute("name").as_string();
    enemy.texture = resource_manager::GetTexture(
        object_node.attribute("texture").as_string());
    enemy.max_health = object_node.attribute("max_health").as_float();
    enemy.health = enemy.max_health;
    enemy.max_stamina = object_node.attribute("max_stamina").as_float();
    enemy.stamina = enemy.max_stamina;
    enemy.damage = object_node.attribute("damage").as_float();
    for (auto skill_node : object_node.children("Skill")) {
      Skill skill;
      skill.name = skill_node.attribute("name").as_string();
      skill.damage = skill_node.attribute("damage").as_float();
      skill.health_used = skill_node.attribute("health_used").as_float();
      skill.stamina_used = skill_node.attribute("stamina_used").as_float();
      enemy.skills.emplace_back(skill);
    }
    default_enemies.push_back(enemy);
  }
  core::Log(std::format("Loaded {} enemy presets", default_enemies.size()), "Enemy");
}

}  // namespace

Enemy game::CreateEnemyFromName(std::string_view name, int designated_enemy_id) {
  static int last_uid = 0;
  // load default enemies if not already loaded
  if (default_enemies.empty()) {
    std::call_once(default_enemies_initialized, LoadEnemies);
  }

  for (const auto& enemy : default_enemies) {
    core::Log(std::format("Found enemy preset: {}", enemy.name), "Enemy");
    if (enemy.name == name) {
      Enemy return_enemy = enemy;
      last_uid = std::max(last_uid, designated_enemy_id);
      if (designated_enemy_id != 0) {
        return_enemy.uid = designated_enemy_id;
      } else {
        return_enemy.uid = ++last_uid;
      }
      return return_enemy;
    }
  }
  throw core::Error("Enemy type not found: " + std::string(name), "Enemy");
}

void game::UpdateBattleTriggers(entt::registry& registry,
                                SceneManager& scene_manager) {
  auto view = registry.view<BattleTrigger>();
  auto player_entity = registry.view<PlayerSkills>().front();
  auto player_skills = registry.get<PlayerSkills>(player_entity);
  auto player_health = registry.get<Health>(player_entity);
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
        scene_manager.PushScene(std::make_unique<BattleScene>(
            scene_manager, damager.enemies, player_skills, player_health));
      }
    }
  }
}
