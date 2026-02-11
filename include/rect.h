#pragma once
#include <glm/glm.hpp>
#include "shader.h"

// Renders a colored rectangle in screen space
struct Rect {
  glm::vec2 position;
  glm::vec2 scale;
  glm::vec4 color;
  void Render(unsigned int vertex_attrib, const Shader& shader, const glm::mat4& projection, const glm::mat4& view);
};