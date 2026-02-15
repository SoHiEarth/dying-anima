#pragma once
#include <box2d/box2d.h>

#include <glm/glm.hpp>

#include "texture.h"

struct Object {
  glm::vec2 position = glm::vec2(0.0f);
  glm::vec2 scale = glm::vec2(1.0f);
  float rotation = 0.0f;
  std::string texture_name;
  b2BodyId body;
};