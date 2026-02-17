#pragma once
#include <box2d/box2d.h>

struct Health {
  float health{100.0f};
};

struct PlayerSpeed {
  float max_speed = 2.0f;
  float speed = 2.0f / 8.0f;
  float deceleration = 2.0f / (2.0f + 30.0f);
  float boost_speed = 50.0f;
  float jump_impulse = 50.0f;
};

bool IsOnGround(b2BodyId body);
