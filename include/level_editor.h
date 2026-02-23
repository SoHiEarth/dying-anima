#pragma once
#include "core/scene.h"
#include "shader.h"

struct LevelEditor : public Scene {
 public:
	 using Scene::Scene;
	 void Init() override;
	 void Quit() override;
	 void HandleInput() override;
	 void Update(float dt) override;
   void Render(GameWindow& window) override;
   bool is_transparent = false;
   Shader* rect_shader = nullptr;
};