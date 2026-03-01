#ifndef INTRO_H
#define INTRO_H

#include <vector>
#include <string>
#include "core/scene.h"
#include "font.h"
#include "shader.h"

struct IntroScene : public Scene {
  std::string Name() override { return "IntroScene"; };
	Shader* text_shader;
  Shader* rect_shader;
  Font* font;
  std::vector<std::string> intro_text = {
    "In The City of Mer,",
    "the city of lost souls - named \"Anima\",",
    "awoke a hero.\nA hero who was born early.",
    "The City's name was not Mer, but the passage of time eroded its name."
  };
  using Scene::Scene;
	void Init() override;
	void Quit() override;
	void HandleInput() override;
	void Update(float dt) override;
  void Render(GameWindow& window) override;
  bool IsTransparent() { return true; };
};

#endif  // INTRO_H