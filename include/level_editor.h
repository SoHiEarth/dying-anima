#pragma once
#include "core/scene.h"
#include "shader.h"
#include <entt/entt.hpp>

struct LevelEditor : public Scene {
 public:
   std::string Name() override { return "LevelEditor"; };
	 using Scene::Scene;
	 void Init() override;
	 void Quit() override;
	 void HandleInput() override;
	 void Update(float dt) override;
   void Render(GameWindow& window) override;
   bool IsTransparent() { return false; };
   Shader* rect_shader = nullptr, *sprite_shader = nullptr;
   entt::registry registry;
};