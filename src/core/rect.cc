#include "core/rect.h"

#include <memory>

#include "core/camera.h"
#include "core/quad.h"
#include "core/shader.h"
#include "core/window.h"
#include "util/calculate.h"

namespace {
glm::mat4 last_projection, last_view, last_vp;
}

void Rect::Render(const std::shared_ptr<Shader>& shader) const {
  shader->Use();
  if (last_projection != GetGameWindow().GetProjection() ||
      last_view != GetCamera().GetView()) {
    last_projection = GetGameWindow().GetProjection();
    last_view = GetCamera().GetView();
    last_vp = last_projection * last_view;
  }
  shader->SetUniform("mvp",
                     last_vp * CalculateModelMatrix(position, 0.0F, scale));
  shader->SetUniform("rect_color", color);
  core::quad::Render(core::quad::QuadType::kQuadOnly);
}
