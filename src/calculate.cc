#include "calculate.h"

#include "transform.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

glm::mat4 CalculateModelMatrix(const Transform &transform) {
  return CalculateModelMatrix(transform.position, transform.rotation,
                              transform.scale);
}

glm::mat4 CalculateModelMatrix(const glm::vec2 &position, float rotation,
                               const glm::vec2 &scale) {
  /*
  * Backup use: cleaner but slower
  auto model = glm::translate(glm::mat4(1.0f), glm::vec3(position, 0.0f));
  if (rotation != 0.0f) {
    model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));
  }
  if (scale.x != 1.0f || scale.y != 1.0f) {
    model = glm::scale(model, glm::vec3(scale, 1.0f));
  }
  */

  glm::mat4 model(1.0f);
  if (rotation != 0.0f) {
    float r = glm::radians(rotation);
    float c = std::cos(r);
    float s = std::sin(r);
    model[0][0] = c * scale.x;
    model[0][1] = s * scale.x;
    model[1][0] = -s * scale.y;
    model[1][1] = c * scale.y;
  } else {
    model[0][0] = scale.x;
    model[1][1] = scale.y;
  }
  model[3][0] = position.x;
  model[3][1] = position.y;
  return model;
}
