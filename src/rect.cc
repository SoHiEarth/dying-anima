#include <glad/glad.h>
// Code block

#include "rect.h"

#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>

#include "camera.h"
#include "core/quad.h"
#include "shader.h"
#include "window.h"
#include "calculate.h"

void Rect::Render(const Shader *shader) {
  shader->Use();
  auto mvp = GetGameWindow().GetProjection() * GetCamera().GetView() *
             CalculateModelMatrix(position, 0.0f, scale);
  shader->SetUniform("mvp", mvp);
  shader->SetUniform("rect_color", color);
  core::quad::Render(core::quad::QuadType::QUAD_ONLY);
}
