#pragma once
#include <glm/glm.hpp>
#include "texture.h"

struct Player
{
  glm::vec3 position{0.0f, 0.0f, 1.0f};
  glm::vec3 rotation{0.0f, 0.0f, 0.0f};
  glm::vec3 scale{1.0f, 2.0f, 1.0f};
  Texture *texture = nullptr;
  float max_speed = 2.0f;
  float speed = 2.0f / 8.0f;
  float deceleration = 2.0f / (2.0f + 30.0f);
  float boost_speed = 10.0f;
  float jump_impulse = 5.0f;
  float health{100.0f};
};