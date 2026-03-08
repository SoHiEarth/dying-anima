#ifndef GAME_ENEMY_H
#define GAME_ENEMY_H

#include <entt/entt.hpp>
#include <glm/glm.hpp>

struct PlayerDamager {
  int damage = 5.0f;
  float hitbox_radius = 0.5f;
  float knockback = 1.0f;
  glm::vec2 knockback_direction = glm::vec2(1.0f, 0.0f);
  float time_until_next_hit = 2.0f;

  // Internal use
  float accumulator = 0.0f;
};

namespace game {
void UpdatePlayerDamagers(entt::registry& registry, float delta_time);
}

#endif  // GAME_ENEMY_H