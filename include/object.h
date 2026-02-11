#pragma once
#include <box2d/box2d.h>
#include <glm/glm.hpp>
#include "texture.h"

struct Object {
  glm::vec3 position;
  glm::vec3 scale;
  glm::vec3 rotation;
  glm::vec4 color;
  std::string texture_name;
  b2BodyId body;
};