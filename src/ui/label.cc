#include "ui/elements.h"

void ui::Label::Update(const glm::ivec2& mouse_pos, bool mouse_pressed) {
  if (IsHovered(mouse_pos)) {
    hovered = true;
  } else {
    hovered = false;
  }
}

void ui::Label::Render(const Shader* text_shader, const Shader* rect_shader) {
  auto final_color = hovered ? color * 1.2f : color;
  if (font) {
    font->RenderUIAtHeight(text, position, size.x, final_color, text_shader);
  }
}

bool ui::Label::IsHovered(const glm::ivec2& mouse_pos) const {
  return mouse_pos.x >= position.x && mouse_pos.x <= position.x + size.x &&
         mouse_pos.y >= position.y && mouse_pos.y <= position.y + size.y;
}