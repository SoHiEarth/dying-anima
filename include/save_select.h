#ifndef SAVE_SELECT_H
#define SAVE_SELECT_H

#include "core/font.h"
#include "core/scene.h"
#include "core/shader.h"
#include "ui/elements.h"

struct SaveSelect : public Scene {
  std::string Name() override { return "SaveSelectScene"; };
  std::shared_ptr<Font> font;
  std::shared_ptr<Shader> text_shader;
  std::shared_ptr<Shader> rect_shader;
  ui::VerticalLayout layout;
  using Scene::Scene;
  void Init() override;
  void Quit() override;
  void Update(double dt) override;
  void Render(GameWindow& window) override;
  bool IsTransparent() override { return false; };
};

#endif  // SAVE_SELECT_H
