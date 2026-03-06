#pragma once
#include "core/scene.h"
#include "core/font.h"
#include "core/shader.h"
#include "core/texture.h"

struct MenuScene : public Scene {
  std::string Name() override { return "MenuScene"; };
  std::shared_ptr<Font> font;
  std::shared_ptr<Shader> text_shader;
  std::shared_ptr<Shader> sprite_shader;
  std::shared_ptr<Texture> banner_texture;
  std::shared_ptr<Texture> selected_texture;
  using Scene::Scene;
  void Init() override;
  void Quit() override;
  void Update(double dt) override;
  void Render(GameWindow& window) override;
  bool IsTransparent() { return false; };
};