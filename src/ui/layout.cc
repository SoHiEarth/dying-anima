#include "core/window.h"
#include "ui/elements.h"

ui::Element* ui::Layout::AddElement(std::unique_ptr<ui::Element> element) {
  elements_.push_back(std::move(element));
  return elements_.back().get();
}

void ui::Layout::Clear() { elements_.clear(); }

void ui::Layout::Update(const glm::ivec2& mouse_pos, bool mouse_pressed) {
  // flip mouse_pos y coordinate to match OpenGL's coordinate system
  auto window_height = GetGameWindow().height;
  for (const auto& element : elements_) {
    element->Update({mouse_pos.x, window_height - mouse_pos.y}, mouse_pressed);
  }
}

void ui::Layout::Render(const std::shared_ptr<Shader> text_shader,
                        const std::shared_ptr<Shader> rect_shader) {
  for (const auto& element : elements_) {
    element->Render(text_shader, rect_shader);
  }
}

void ui::Layout::UpdateLayout() { CalculateLayout(); }

void ui::VerticalLayout::CalculateLayout() {
  int current_y = position.y + padding_[0];
  for (const auto& element : elements_) {
    auto element_size = element->GetSize();
    element->position = {position.x + padding_[3], current_y};
    current_y += element_size.y + spacing_;
  }
}

int ui::VerticalLayout::GetLayoutSize() {
  int total_size =
      padding_[0] + padding_[2] + (spacing_ * (static_cast<int>(elements_.size()) - 1));
  for (const auto& element : elements_) {
    total_size += element->GetSize().y;
  }
  return total_size;
}

void ui::HorizontalLayout::CalculateLayout() {
  int current_x = position.x + padding_[3];
  for (const auto& element : elements_) {
    auto element_size = element->GetSize();
    element->position = {current_x, position.y + padding_[0]};
    current_x += element_size.x + spacing_;
  }
}

int ui::HorizontalLayout::GetLayoutSize() {
  int total_size =
      padding_[1] + padding_[3] + (spacing_ * (static_cast<int>(elements_.size()) - 1));
  for (const auto& element : elements_) {
    total_size += element->GetSize().x;
  }
  return total_size;
}

ui::Label::Label(std::string text, std::shared_ptr<Font> font)
    : text_(std::move(text)), font_(std::move(font)) {}

ui::Button::Button(std::string text, std::shared_ptr<Font> font,
                   std::function<void()> on_click)
    : text_(std::move(text)),
      font_(std::move(font)),
      on_click_(std::move(on_click)) {}

