#ifndef INTRO_H
#define INTRO_H

#include <memory>
#include <string>
#include <vector>

#include "core/font.h"
#include "core/scene.h"
#include "core/shader.h"

struct IntroScene : public Scene {
  std::string Name() override { return "IntroScene"; };
  std::shared_ptr<Shader> text_shader;
  std::shared_ptr<Shader> rect_shader;
  std::shared_ptr<Font> font;
  std::vector<std::string> intro_text = {
      "In The City of Mer,", "the city of lost souls - named \"Anima\",",
      "awoke a hero.", "The City's name was not Mer,",
      "but the passage of time eroded its name."};
  using Scene::Scene;
  void Init() override;
  void Quit() override;
  void Update(double dt) override;
  void Render(GameWindow& window) override;
  bool IsTransparent() override { return true; };
};

#endif  // INTRO_H