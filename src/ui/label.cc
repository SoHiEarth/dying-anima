#include "ui/elements.h"

void ui::Label::Update(const glm::ivec2& mouse_pos, bool /* mouse_pressed */) {
  size.x = static_cast<int>(font_->GetWidth(text_) * font_->GetWidthScale(text_, static_cast<float>(size.y)));
  hovered_ = IsHovered(mouse_pos);
}

void ui::Label::Render(const std::shared_ptr<Shader> text_shader,
                       const std::shared_ptr<Shader> /* rect_shader */) {
  if (font_) {
    font_->RenderUIAtHeight(text_, position, static_cast<float>(size.y),
                           hovered_ ? color_ * 1.2F : color_, text_shader);
  }
}

bool ui::Label::IsHovered(const glm::ivec2& mouse_pos) const {
  return mouse_pos.x >= position.x && mouse_pos.x <= position.x + size.x &&
         mouse_pos.y >= position.y && mouse_pos.y <= position.y + size.y;
}
