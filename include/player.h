#pragma once
#include <glm/glm.hpp>

#include "object.h"

struct Player : public Object {
  float max_speed = 2.0f;
  float speed = 2.0f / 8.0f;
  float deceleration = 2.0f / (2.0f + 30.0f);
  float boost_speed = 100.0f;
  float jump_impulse = 5.0f;
  float health{100.0f};
  bool IsOnGround();
};
