#pragma once
#include <box2d/box2d.h>

struct Health {
  float health{100.0f};
};

struct PlayerSpeed {
  float max_speed = 3.0f;
  float max_boost_speed = 5.0f;
  float air_control_multiplier = 0.85f;
  float speed = 0.25f;
  float deceleration = 0.325f;
  float boost_speed = 1.75f;
  float jump_impulse = 0.275f;
};

bool IsOnGround(b2BodyId body);
