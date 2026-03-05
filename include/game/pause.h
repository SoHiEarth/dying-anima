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
  std::shared_ptr<Font> title_font;
	std::shared_ptr<Font> ui_font;
  std::shared_ptr<Shader> rect_shader;
        std::shared_ptr<Shader> text_shader;
  double time_since_open = 0.0f;
};

#endif  // PAUSE_H