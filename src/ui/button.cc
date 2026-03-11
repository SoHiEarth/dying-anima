#include "core/rect.h"
#include "ui/elements.h"

void ui::Button::Update(const glm::ivec2& mouse_pos, bool mouse_pressed) {
  size.x =
      static_cast<int>(font_->GetWidth(text_) *
                       font_->GetWidthScale(text_, static_cast<float>(size.y)));
  if (IsHovered(mouse_pos)) {
    hovered_ = true;
    if (mouse_pressed && on_click_) {
      on_click_();
    }
  } else {
    hovered_ = false;
  }
}

void ui::Button::Render(const std::shared_ptr<Shader> text_shader,
                        const std::shared_ptr<Shader> rect_shader) {
  if (rect_shader) {
    Rect background;
    background.position = {GetPosition().x + (size.x / 2.0F),
                           GetPosition().y + (size.y / 2.0F)};
    background.scale = size;
    background.color = {hovered_ ? hover_color_ : background_color_};
    background.Render(rect_shader);
  }

  if (text_shader && font_) {
    font_->RenderUIAtHeight(text_, GetPosition(), static_cast<float>(size.y),
                            {1.0F, 1.0F, 1.0F}, text_shader);
  }
}

bool ui::Button::IsHovered(const glm::ivec2& mouse_pos) const {
  return mouse_pos.x >= position.x && mouse_pos.x <= position.x + size.x &&
         mouse_pos.y >= position.y && mouse_pos.y <= position.y + size.y;
}
