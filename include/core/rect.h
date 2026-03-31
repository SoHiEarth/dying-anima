#ifndef CORE_RECT_H
#define CORE_RECT_H

#include <glm/glm.hpp>
#include <memory>

#include "core/shader.h"

// Renders a colored rectangle in screen space
struct Rect {
  glm::vec2 position;
  glm::vec2 scale;
  glm::vec4 color;
  void Render(const std::shared_ptr<Shader>& shader) const;
};

#endif  // CORE_RECT_H
