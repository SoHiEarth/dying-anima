#pragma once
#include <glm/glm.hpp>

glm::mat4 CalculateModelMatrix(
  const glm::vec3& position,
  const glm::vec3& rotation,
  const glm::vec3& scale
);