#pragma once
#include "core/scene.h"
#include "core/font.h"
#include "core/texture.h"
#include "core/shader.h"
#include "core/transform.h"
#include "game/player.h"
#include <entt/entt.hpp>
#include "saves.h"

namespace game {
extern SaveData save_data;
}

struct GameScene : public Scene {
 public:
  using Scene::Scene;
  std::string Name() override { return "GameScene"; };
  void Init() override;
  void Quit() override;
  void Update(double dt) override;
  void Render(GameWindow& window) override;
  bool IsTransparent() { return false; };
  entt::registry registry;
  std::shared_ptr<Shader> sprite_shader, rect_shader, text_shader;
  std::shared_ptr<Font> special_font, title_font, ui_font;
  const float physics_time_step = 1.0f / 60.0f;
  float physics_accumulator = 0.0f;
  entt::entity player;
  b2BodyId player_body;
};