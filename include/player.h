#pragma once
#include <glm/glm.hpp>
#include "texture.h"

struct Player {
  glm::vec3 position{0.0f, 0.0f, 0.0f};
  Texture* texture = nullptr;
  float max_speed = 150.0f;
  float speed = 150.0f/8.0f;
  float deceleration = 150.0f/(150.0f + 30.0f);
  float health{100.0f};
};