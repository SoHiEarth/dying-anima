#ifndef GAME_PLAYER_H
#define GAME_PLAYER_H

#include <box2d/box2d.h>

#include "game/skill.h"

struct Health {
  float max_health{100.0F};
  float max_stamina{100.0F};
  float health{max_health};
  float stamina{max_stamina};
};

struct PlayerSpeed {
  float max_speed = 3.0F;
  float max_boost_speed = 5.0F;
  float air_control_multiplier = 0.85F;
  float speed = 0.25F;
  float deceleration = 0.325F;
  float boost_speed = 1.75F;
  float jump_impulse = 10.75F;
};

struct PlayerSkills {
  std::vector<Skill> skills;
};

bool IsOnGround(b2BodyId body);

namespace game {
void RenderPlayerInfo();
}  // namespace game

#endif  // GAME_PLAYER_H
