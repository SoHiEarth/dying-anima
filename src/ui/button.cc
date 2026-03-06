#include "ui/elements.h"
#include "core/rect.h"
#include <print>

void ui::Button::Update(const glm::ivec2& mouse_pos, bool mouse_pressed) {
  size.x = font->GetWidthScale(text, size.y);
  if (IsHovered(mouse_pos)) {
    hovered = true;
    if (mouse_pressed && on_click) { on_click(); }
  } else {
    hovered = false;
  }
}

void ui::Button::Render(const std::shared_ptr<Shader> text_shader,
                        const std::shared_ptr<Shader> rect_shader) {
  if (rect_shader) {
    Rect background;
    background.position = {GetPosition().x + size.x / 2.0f,
                           GetPosition().y + size.y / 2.0f};
    background.scale = size;
    background.color = {hovered ? hover_color : background_color, 1.0f};
    background.Render(rect_shader);
  }

  if (text_shader && font) {
    font->RenderUIAtHeight(text, GetPosition(), size.y, {1.0f, 1.0f, 1.0f}, text_shader);
  }
}

bool ui::Button::IsHovered(const glm::ivec2& mouse_pos) const {
  return mouse_pos.x >= position.x && mouse_pos.x <= position.x + size.x &&
         mouse_pos.y >= position.y && mouse_pos.y <= position.y + size.y;
}