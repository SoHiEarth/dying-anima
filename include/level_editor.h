#pragma once
#include <entt/entt.hpp>

#include "core/scene.h"
#include "core/shader.h"

struct LevelEditor : public Scene {
 public:
  std::string Name() override { return "LevelEditor"; };
  using Scene::Scene;
  void Init() override;
  void Quit() override;
  void Update(double dt) override;
  void Render(GameWindow& window) override;
  bool IsTransparent() { return false; };
  std::shared_ptr<Shader> rect_shader;
  std::shared_ptr<Shader> sprite_shader;
  entt::registry registry;
};