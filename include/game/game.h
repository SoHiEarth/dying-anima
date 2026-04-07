#ifndef GAME_GAME_H
#define GAME_GAME_H

#include <entt/entt.hpp>

#include "core/font.h"
#include "core/scene.h"
#include "core/shader.h"
#include "core/texture.h"
#include "game/log.h"
#include "saves.h"

namespace game {
extern SaveData save_data;
extern entt::registry registry;
extern game::Log player_log;
extern entt::entity player;
}

struct GameScene : public Scene {
 public:
  using Scene::Scene;
  std::string Name() override { return "GameScene"; };
  void Init() override;
  void Quit() override;
  void Update(double dt) override;
  void Render(GameWindow& window) override;
  bool IsTransparent() override { return false; };
  std::shared_ptr<Shader> sprite_shader, rect_shader, text_shader;
  std::shared_ptr<Font> special_font, title_font, ui_font;
  const float physics_time_step = 1.0F / 60.0F;
  float physics_accumulator = 0.0F;
  b2BodyId player_body;
};

#endif
