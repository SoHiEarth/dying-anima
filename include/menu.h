#pragma once
#include "core/font.h"
#include "core/scene.h"
#include "core/shader.h"
#include "core/texture.h"
#include "ui/elements.h"

struct MenuScene : public Scene {
  std::string Name() override { return "MenuScene"; };
  std::shared_ptr<Font> font;
  std::shared_ptr<Shader> text_shader;
  std::shared_ptr<Shader> rect_shader;
  ui::VerticalLayout menu_layout;
  using Scene::Scene;
  void Init() override;
  void Quit() override;
  void Update(double dt) override;
  void Render(GameWindow& window) override;
  bool IsTransparent() override { return false; };
};
