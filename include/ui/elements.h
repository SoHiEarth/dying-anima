#ifndef UI_BUTTON_H
#define UI_BUTTON_H

#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <vector>

#include "font.h"
#include "shader.h"

namespace ui {
struct Element {
 public:
  virtual ~Element() = default;
  virtual void Update(const glm::ivec2& mouse_pos, bool mouse_pressed) = 0;
  virtual void Render(const std::shared_ptr<Shader> text_shader, const std::shared_ptr<Shader> rect_shader) = 0;
  glm::ivec2 GetPosition() const { return position; }
  glm::ivec2 GetSize() const { return size; }
  void SetPosition(const glm::ivec2& new_position) { position = new_position; }
  void SetSize(const glm::ivec2& new_size) { size = new_size; }
  glm::ivec2 position{0, 0};
  glm::ivec2 size{ 20, 20 };
};

struct Button : Element {
 public:
  Button(const std::string& text, std::shared_ptr<Font> font, std::function<void()> on_click)
      : text(text), font(font), on_click(on_click) {}
  void Update(const glm::ivec2& mouse_pos, bool mouse_pressed) override;
  void Render(const std::shared_ptr<Shader> text_shader, const std::shared_ptr<Shader> rect_shader) override;
  void SetText(std::string_view new_text) { text = new_text; }
  void SetTextColor(const glm::vec3& new_color) { text_color = new_color; }
  void SetHoverColor(const glm::vec3& new_color) { hover_color = new_color; }
  void SetBackgroundColor(const glm::vec3& new_color) {
    background_color = new_color;
  }
  void SetBorderColor(const glm::vec3& new_color) { border_color = new_color; }
  void SetBorderThickness(float new_thickness) {
    border_thickness = new_thickness;
  }
  void SetOnClick(std::function<void()> new_on_click) {
    on_click = new_on_click;
  }
  bool IsHovered(const glm::ivec2& mouse_pos) const;

 private:
  std::string text;
  std::shared_ptr<Font> font;
  std::function<void()> on_click;
  glm::vec3 background_color{0.2f, 0.2f, 0.2f};
  glm::vec3 hover_color{0.3f, 0.3f, 0.3f};
  glm::vec3 text_color{1.0f, 1.0f, 1.0f};
  float border_thickness{2.0f};
  glm::vec3 border_color{1.0f, 1.0f, 1.0f};
  bool hovered{false};
};

struct Label : Element {
 public:
  Label(const std::string& text, std::shared_ptr<Font> font) : text(text), font(font) {}
  void Update(const glm::ivec2& mouse_pos, bool mouse_pressed) override;
  void Render(const std::shared_ptr<Shader> text_shader, const std::shared_ptr<Shader> rect_shader) override;
  void SetText(std::string_view new_text) { text = new_text; }
  void SetFont(std::shared_ptr<Font> new_font) { font = new_font; }
  void SetColor(const glm::vec3& new_color) { color = new_color; }

 private:
  std::string text;
  std::shared_ptr<Font> font;
  glm::vec3 color{1.0f, 1.0f, 1.0f};
  bool IsHovered(const glm::ivec2& mouse_pos) const;
  bool hovered{false};
};

struct Layout : Element {
 public:
  // Add an element to the layout. The layout takes ownership of the element.
  ui::Element* AddElement(std::unique_ptr<Element> element);
  // Clear all elements from the layout.
  void Clear();
  // Call Update on all child elements.
  void Update(const glm::ivec2& mouse_pos, bool mouse_pressed) override;
  // Call Render on all child elements.
  void Render(const std::shared_ptr<Shader> text_shader, const std::shared_ptr<Shader> rect_shader) override;
  // Set the spacing between elements in the layout.
  void SetSpacing(int new_spacing) { spacing = new_spacing; }
  // Set the padding around the edges of the layout (top, right, bottom, left).
  void SetPadding(int top, int right, int bottom, int left) {
    padding = {top, right, bottom, left};
  }
  // Recalculate the layout of child elements. Should be called after
  // adding/removing elements or changing spacing/padding.
  void UpdateLayout();

 protected:
  std::vector<std::unique_ptr<Element>> elements;
  int spacing{10};
  glm::ivec4 padding{10, 10, 10, 10}; // top, right, bottom, left
  // Implmentation of layout calculation
  virtual void CalculateLayout() = 0;
};

struct VerticalLayout : Layout {
 protected:
  void CalculateLayout() override;
};

struct HorizontalLayout : Layout {
 protected:
  void CalculateLayout() override;
};
}  // namespace ui

#endif  // UI_BUTTON_H