#include <glad/glad.h>
// Code block

#include "rect.h"

#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>

#include "core/quad.h"

void Rect::Render(const Shader &shader, const glm::mat4 &projection,
                  const glm::mat4 &view) {
  shader.Use();
  shader.SetUniform("projection", projection);
  shader.SetUniform("view", view);
  auto model = glm::translate(glm::mat4(1.0f), glm::vec3(position, 0.0f));
  model = glm::scale(model, glm::vec3(scale, 1.0f));
  shader.SetUniform("model", model);
  shader.SetUniform("rect_color", color);
  core::quad::Render(core::quad::QuadType::QUAD_ONLY);
}
