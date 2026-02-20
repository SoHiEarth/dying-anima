#include "rect.h"
#include "camera.h"
#include "core/quad.h"
#include "shader.h"
#include "window.h"
#include "calculate.h"

static glm::mat4 last_projection, last_view, last_vp;

void Rect::Render(const Shader *shader) {
  shader->Use();
  if (last_projection != GetGameWindow().GetProjection() ||
      last_view != GetCamera().GetView()) {
    last_projection = GetGameWindow().GetProjection();
    last_view = GetCamera().GetView();
    last_vp = last_projection * last_view;
  }
  shader->SetUniform("mvp", last_vp * CalculateModelMatrix(position, 0.0f, scale));
  shader->SetUniform("rect_color", color);
  core::quad::Render(core::quad::QuadType::QUAD_ONLY);
}
