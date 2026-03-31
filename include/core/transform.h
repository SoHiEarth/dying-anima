#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <glm/glm.hpp>

struct Transform {
  glm::vec2 position{0.0F, 0.0F};
  float z_index = 0.0F;
  float rotation = 0.0F;
  glm::vec2 scale{1.0F, 1.0F};
};

#endif  // TRANSFORM_H
