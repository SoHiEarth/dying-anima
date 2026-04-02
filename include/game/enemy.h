#ifndef GAME_ENEMY_H
#define GAME_ENEMY_H

#include <entt/entt.hpp>
#include <glm/glm.hpp>

#include "core/scene.h"
#include "game/skill.h"

struct Enemy {
  std::string name;
  float health;
  float max_health;
  float stamina;
  float max_stamina;
  float damage;
  std::vector<Skill> skills;
};

struct BattleTrigger {
  std::vector<Enemy> enemies;
  float hitbox_radius = 5.0F;
};

namespace game {
Enemy CreateEnemyFromName(std::string_view name);
void UpdateBattleTriggers(entt::registry& registry,
                          SceneManager& scene_manager);
}  // namespace game

#endif  // GAME_ENEMY_H
