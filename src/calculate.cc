#include "calculate.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

glm::mat4 CalculateModelMatrix(
  const glm::vec3& position,
  const glm::vec3& rotation,
  const glm::vec3& scale
) {
  glm::mat4 model = glm::mat4(1.0f);
  model = glm::translate(model, position);
  model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
  model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
  model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
  model = glm::scale(model, scale);
  return model;
}