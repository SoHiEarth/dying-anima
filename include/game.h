#pragma once
#include "core/scene.h"
#include "font.h"
#include "texture.h"
#include "shader.h"
#include "transform.h"
#include "player.h"
#include <entt/entt.hpp>

struct GameScene : public Scene {
 public:
  using Scene::Scene;
  void Init() override;
  void Quit() override;
  void HandleInput() override;
  void Update(float dt) override;
  void Render(GameWindow& window) override;
  bool is_transparent = false;
  entt::registry registry;
  Shader *sprite_shader, *rect_shader, *text_shader;
  Font *special_font, *title_font, *ui_font;
  const float physics_time_step = 1.0f / 60.0f;
  float physics_accumulator = 0.0f;
  entt::entity player;
  b2BodyId player_body;
};