#include <glad/glad.h>
// Code block

#include <GLFW/glfw3.h>
#include <ft2build.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "menu.h"
#include FT_FREETYPE_H

#include <algorithm>

#include "core/atlas.h"
#include "core/camera.h"
#include "core/font.h"
#include "core/input.h"
#include "core/quad.h"
#include "core/resource_manager.h"
#include "core/shader.h"
#include "core/state.h"
#include "core/texture.h"
#include "core/window.h"
#include "game/game.h"
#include "game/intro.h"
#include "game/progression.h"
#include "level_editor.h"
#include "ui/elements.h"
#include "util/calculate.h"
constexpr int LABEL_SIZE_Y = 36;

void MenuScene::Init() {
  glfwSwapInterval(1);  // Save resources
  font = ResourceManager::GetFont("menu.ui").font;
  text_shader = ResourceManager::GetShader("Text").shader;
  rect_shader = ResourceManager::GetShader("Rect").shader;
  text_shader->Use();
  text_shader->SetUniform("character", 0);
#ifndef NDEBUG
  menu_layout.AddElement(std::make_unique<ui::Button>(
              "Editor", font,
              [this]() {
                scene_manager.PushScene(
                    std::make_unique<LevelEditor>(scene_manager));
              }))
          ->size.y = LABEL_SIZE_Y;
#endif
  menu_layout.AddElement(std::make_unique<ui::Button>(
              "Exit", font, [this]() { scene_manager.PopScene(); }))
          ->size.y = LABEL_SIZE_Y;
  menu_layout.AddElement(std::make_unique<ui::Button>(
                            "Play", font,
                            [this]() {
                              scene_manager.PushScene(
                                  std::make_unique<GameScene>(scene_manager));
                            }))
                        ->size.y = LABEL_SIZE_Y;
  menu_layout.AddElement(std::make_unique<ui::Label>("Dying Anima", font))
          ->size.y = LABEL_SIZE_Y * 2;
  menu_layout.SetSpacing(LABEL_SIZE_Y);
}

int focus_index = 0;
void MenuScene::Update(double /* dt */) {
  if (core::input::IsKeyPressedThisFrame(GLFW_KEY_ESCAPE)) {
    scene_manager.PopScene();
  }
  if (core::input::IsKeyPressedThisFrame(GLFW_KEY_ENTER)) {
    switch (static_cast<AppState>(focus_index)) {
      case AppState::PLAYING:
        scene_manager.PopScene();
        scene_manager.PushScene(std::make_unique<GameScene>(scene_manager));
        if (std::find(game::save_data.completion_markers.begin(),
                      game::save_data.completion_markers.end(),
                      Progression::INTRO_COMPLETE_MARKER) ==
            game::save_data.completion_markers.end()) {
          scene_manager.PushScene(std::make_unique<IntroScene>(scene_manager));
        }
        break;
      case AppState::EXIT:
        glfwSetWindowShouldClose(GetGameWindow().window, true);
        break;
      case AppState::LEVEL_EDITOR:
        scene_manager.PopScene();
        scene_manager.PushScene(std::make_unique<LevelEditor>(scene_manager));
        break;
    }
  }
  if (core::input::IsKeyPressedThisFrame(GLFW_KEY_LEFT)) {
    focus_index = std::max(focus_index - 1, 0);
  }
  if (core::input::IsKeyPressedThisFrame(GLFW_KEY_RIGHT)) {
#ifdef NDEBUG
    const int max_focus = 1;
#else
    const int max_focus = 2;
#endif
    focus_index = std::min(focus_index + 1, max_focus);
  }
  glm::dvec2 mouse_pos{};
  glfwGetCursorPos(GetGameWindow().window, &mouse_pos.x, &mouse_pos.y);
  menu_layout.SetPosition(
      {30, (GetGameWindow().height - menu_layout.GetLayoutSize()) / 2});
  menu_layout.Update(mouse_pos,
                     core::input::IsKeyPressed(GLFW_MOUSE_BUTTON_LEFT));
}

void MenuScene::Render(GameWindow& window) {
  window.SetProjection(ProjectionType::SCREEN_SPACE);
  GetCamera().SetType(CameraType::UI);
  menu_layout.Render(text_shader, rect_shader);
}

void MenuScene::Quit() {
  glfwSwapInterval(0);  // Psycho mode
}