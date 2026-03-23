#pragma once
#include <box2d/box2d.h>

struct Health {
  float health{100.0F};
};

struct PlayerSpeed {
  float max_speed = 3.0F;
  float max_boost_speed = 5.0F;
  float air_control_multiplier = 0.85F;
  float speed = 0.25F;
  float deceleration = 0.325F;
  float boost_speed = 1.75F;
  float jump_impulse = 8.75F;
};

bool IsOnGround(b2BodyId body);

namespace game {
void RenderPlayerInfo();
}