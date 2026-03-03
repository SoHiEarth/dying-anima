#ifndef PAUSE_H
#define PAUSE_H

#include "core/scene.h"
#include "font.h"

struct PauseScene : public Scene {
 public:
  using Scene::Scene;
	void Init() override;
	void Quit() override;
  void Update(double dt) override;
  void Render(GameWindow& window) override;
  bool IsTransparent() override { return true; }
 private:
  Font* title_font;
	Font* ui_font;
  Shader* rect_shader;
  Shader* text_shader;
  double time_since_open = 0.0f;
};

#endif  // PAUSE_H