#include "game/enemy.h"

#include "core/transform.h"
#include "game/player.h"

void game::UpdatePlayerDamagers(entt::registry& registry, float delta_time) {
  auto view = registry.view<PlayerDamager>();
  for (auto entity : view) {
    auto& damager = view.get<PlayerDamager>(entity);
    auto& transform = registry.get<Transform>(entity);
    auto health_view = registry.view<Health>();
    for (auto health_entity : health_view) {
      auto& health = health_view.get<Health>(health_entity);
      auto& player_transform = registry.get<Transform>(health_entity);
      float distance =
          glm::distance(transform.position, player_transform.position);
      if (distance < damager.hitbox_radius) {
        damager.accumulator += delta_time;
        if (damager.accumulator >= damager.time_until_next_hit) {
          health.health -= damager.damage;
          damager.accumulator = 0.0f;
        }
      }
    }
  }
}