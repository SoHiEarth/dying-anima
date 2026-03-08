#ifndef UI_BUTTON_H
#define UI_BUTTON_H

#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "core/font.h"
#include "core/shader.h"

namespace ui {
struct Element {
 public:
  virtual ~Element() = default;
  virtual void Update(const glm::ivec2& mouse_pos, bool mouse_pressed) = 0;
  virtual void Render(std::shared_ptr<Shader> text_shader,
                      std::shared_ptr<Shader> rect_shader) = 0;
  glm::ivec2 GetPosition() const { return position; }
  glm::ivec2 GetSize() const { return size; }
  virtual void SetPosition(const glm::ivec2& new_position) {
    position = new_position;
  }
  void SetSize(const glm::ivec2& new_size) { size = new_size; }
  glm::ivec2 position{0, 0};
  glm::ivec2 size{20, 20};
};

struct Button : Element {
 public:
  Button(std::string text, std::shared_ptr<Font> font,
         std::function<void()> on_click);
  void Update(const glm::ivec2& mouse_pos, bool mouse_pressed) override;
  void Render(std::shared_ptr<Shader> text_shader,
              std::shared_ptr<Shader> rect_shader) override;
  void SetText(std::string_view new_text) { text_ = new_text; }
  void SetTextColor(const glm::vec3& new_color) { text_color_ = new_color; }
  void SetHoverColor(const glm::vec4& new_color) { hover_color_ = new_color; }
  void SetBackgroundColor(const glm::vec4& new_color) {
    background_color_ = new_color;
  }
  void SetOnClick(std::function<void()> new_on_click) {
    on_click_ = std::move(new_on_click);
  }
  bool IsHovered(const glm::ivec2& mouse_pos) const;

 private:
  std::string text_;
  std::shared_ptr<Font> font_;
  std::function<void()> on_click_;
  glm::vec4 background_color_{0.0F, 0.0F, 0.0F, 0.7F};
  glm::vec4 hover_color_{0.3F, 0.3F, 0.3F, 0.7F};
  glm::vec3 text_color_{1.0F, 1.0F, 1.0F};
  bool hovered_{false};
};

struct Label : Element {
 public:
  Label(std::string text, std::shared_ptr<Font> font);
  void Update(const glm::ivec2& mouse_pos, bool mouse_pressed) override;
  void Render(std::shared_ptr<Shader> text_shader,
              std::shared_ptr<Shader> rect_shader) override;
  void SetText(std::string_view new_text) { text_ = new_text; }
  void SetFont(std::shared_ptr<Font> new_font) { font_ = std::move(new_font); }
  void SetColor(const glm::vec3& new_color) { color_ = new_color; }

 private:
  std::string text_;
  std::shared_ptr<Font> font_;
  glm::vec3 color_{1.0F, 1.0F, 1.0F};
  bool IsHovered(const glm::ivec2& mouse_pos) const;
  bool hovered_{false};
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
  void Render(std::shared_ptr<Shader> text_shader,
              std::shared_ptr<Shader> rect_shader) override;
  // Set the spacing between elements in the layout.
  void SetSpacing(int new_spacing) { spacing_ = new_spacing; }
  // Set the padding around the edges of the layout (top, right, bottom, left).
  void SetPadding(int top, int right, int bottom, int left) {
    padding_ = {top, right, bottom, left};
  }
  void SetPosition(const glm::ivec2& new_position) override {
    position = new_position;
    UpdateLayout();
  }
  // Recalculate the layout of child elements. Should be called after
  // adding/removing elements or changing spacing/padding.
  void UpdateLayout();

  virtual int GetLayoutSize() = 0;

 protected:
  std::vector<std::unique_ptr<Element>> elements_;
  int spacing_{10};
  glm::ivec4 padding_{10, 10, 10, 10};  // top, right, bottom, left
  // Implmentation of layout calculation
  virtual void CalculateLayout() = 0;
};

struct VerticalLayout : Layout {
  int GetLayoutSize() override;

 protected:
  void CalculateLayout() override;
};

struct HorizontalLayout : Layout {
  int GetLayoutSize() override;

 protected:
  void CalculateLayout() override;
};
}  // namespace ui

#endif  // UI_BUTTON_H
