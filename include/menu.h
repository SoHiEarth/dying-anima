#pragma once
#include "core/scene.h"
#include "font.h"
#include "shader.h"
#include "texture.h"

struct MenuScene : public Scene {
  Font* font = nullptr;
  Shader *text_shader = nullptr, *sprite_shader = nullptr;
  Texture *banner_texture = nullptr, *selected_texture = nullptr;
  using Scene::Scene;
  void Init() override;
  void Quit() override;
  void HandleInput() override;
  void Update(float dt) override;
  void Render(GameWindow& window) override;
  bool is_transparent = false;
};