#ifndef CHARACTER_H
#define CHARACTER_H

#include <entt/entt.hpp>
#include <string>

#include "core/window.h"

struct Character {
  std::string name;
  float health;
  float speed;
};

void UpdateCharacters(entt::registry& registry, float dt);
void RenderCharacters(entt::registry& registry, GameWindow& window);

#endif  // CHARACTER_H
