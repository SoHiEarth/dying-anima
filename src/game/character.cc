#include "game/character.h"

#include "core/physics.h"
#include "game/player.h"

void UpdateCharacters(entt::registry& registry, float dt) {
  auto player_view = registry.view<PlayerSpeed, Health>();
  auto character_view = registry.view<Character>();
  for (auto player_entity : player_view) {
    auto player_body = registry.get<PhysicsBody>(player_entity);
    for (auto character_entity : character_view) {
      if (character_entity == player_entity) continue;
      // auto& character = registry.get<Character>(character_entity);
      auto character_body = registry.get<PhysicsBody>(character_entity);
      if (physics::IsColliding(player_body, character_body)) {
      }
    }
  }
}
