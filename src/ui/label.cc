#include "ui/elements.h"

void ui::Label::Update(const glm::ivec2& mouse_pos, bool /* mouse_pressed */) {
  size.x = font->GetWidth(text) * (int)font->GetWidthScale(text, (float)size.y);
  if (IsHovered(mouse_pos)) {
    hovered = true;
  } else {
    hovered = false;
  }
}

void ui::Label::Render(const std::shared_ptr<Shader> text_shader,
                       const std::shared_ptr<Shader> /* rect_shader */) {
  if (font) {
    font->RenderUIAtHeight(text, position, (float)size.y,
                           hovered ? color * 1.2f : color, text_shader);
  }
}

bool ui::Label::IsHovered(const glm::ivec2& mouse_pos) const {
  return mouse_pos.x >= position.x && mouse_pos.x <= position.x + size.x &&
         mouse_pos.y >= position.y && mouse_pos.y <= position.y + size.y;
}