#include "ui/elements.h"
#include "core/window.h"

ui::Element* ui::Layout::AddElement(std::unique_ptr<ui::Element> element) {
  elements.push_back(std::move(element));
  return elements.back().get();
}

void ui::Layout::Clear() { elements.clear(); }

void ui::Layout::Update(const glm::ivec2& mouse_pos, bool mouse_pressed) {
  // flip mouse_pos y coordinate to match OpenGL's coordinate system
  auto window_height = GetGameWindow().height;
  for (const auto& element : elements) {
    element->Update({mouse_pos.x, window_height - mouse_pos.y}, mouse_pressed);
  }
}

void ui::Layout::Render(const std::shared_ptr<Shader> text_shader,
                        const std::shared_ptr<Shader> rect_shader) {
  for (const auto& element : elements) {
    element->Render(text_shader, rect_shader);
  }
}

void ui::Layout::UpdateLayout() { CalculateLayout(); }

void ui::VerticalLayout::CalculateLayout() {
  int current_y = position.y + padding[0];
  int layout_width = size.x;
  for (const auto& element : elements) {
    auto element_size = element->GetSize();
    element->position = {position.x + padding[3], current_y};
    current_y += element_size.y + spacing;
  }
}

void ui::HorizontalLayout::CalculateLayout() {
  int current_x = position.x + padding[3];
  int layout_height = size.y;
  for (const auto& element : elements) {
    auto element_size = element->GetSize();
    element->position = {current_x, position.y + padding[0]};
    current_x += element_size.x + spacing;
  }
}