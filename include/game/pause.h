#ifndef PAUSE_H
#define PAUSE_H

#include "core/font.h"
#include "core/scene.h"

struct PauseScene : public Scene {
 public:
  using Scene::Scene;
  void Init() override;
  void Quit() override;
  void Update(double dt) override;
  void Render(GameWindow& window) override;
  bool IsTransparent() override { return true; }

 private:
  std::shared_ptr<Font> title_font_;
  std::shared_ptr<Font> ui_font_;
  std::shared_ptr<Shader> rect_shader_;
  std::shared_ptr<Shader> text_shader_;
  double time_since_open_ = 0.0F;
};

#endif  // PAUSE_H