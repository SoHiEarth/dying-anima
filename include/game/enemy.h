#ifndef GAME_ENEMY_H
#define GAME_ENEMY_H

#include <entt/entt.hpp>
#include <glm/glm.hpp>

struct PlayerDamager {
  float damage = 5.0F;
  float hitbox_radius = 0.5F;
  float knockback = 1.0F;
  glm::vec2 knockback_direction = glm::vec2(1.0F, 0.0F);
  float time_until_next_hit = 2.0F;

  // Internal use
  float accumulator = 0.0F;
};

namespace game {
void UpdatePlayerDamagers(entt::registry& registry, float delta_time);
}

#endif  // GAME_ENEMY_H