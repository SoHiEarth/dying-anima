#ifndef PAUSE_H
#define PAUSE_H

#include "core/scene.h"
#include "font.h"

struct PauseScene : public Scene {
 public:
  using Scene::Scene;
	void Init() override;
	void Quit() override;
	void HandleInput() override;
	void Update(float dt) override;
  void Render(GameWindow& window) override;
        bool IsTransparent() override { return true; }
 private:
  Font* title_font;
	Font* ui_font;
  Shader* rect_shader;
  Shader* text_shader;
};

#endif  // PAUSE_H