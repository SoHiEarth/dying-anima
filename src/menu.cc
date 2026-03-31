#include <glad/glad.h>
// Code block

#include <GLFW/glfw3.h>
#include <ft2build.h>
#include <stb_image.h>

#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "menu.h"
#include FT_FREETYPE_H

#include "core/atlas.h"
#include "core/camera.h"
#include "core/input.h"
#include "core/resource_manager.h"
#include "core/state.h"
#include "core/window.h"
#include "game/game.h"
#include "game/intro.h"
#include "level_editor.h"
#include "ui/elements.h"

constexpr int kLabelSizeY = 36;
namespace {
int focus_index = 0;
}
void MenuScene::Init() {
  glfwSwapInterval(1);  // Save resources
  font = resource_manager::GetFont("menu.ui").font;
  text_shader = resource_manager::GetShader("Text").shader;
  rect_shader = resource_manager::GetShader("Rect").shader;
  text_shader->Use();
  text_shader->SetUniform("character", 0);
  // Commented out for demonstration purposes
  // #ifndef NDEBUG
  menu_layout
      .AddElement(std::make_unique<ui::Button>(
          "Editor", font,
          [this]() {
            scene_manager_.PushScene(
                std::make_unique<LevelEditor>(scene_manager_));
          }))
      ->size.y = kLabelSizeY;
  // #endif
  menu_layout
      .AddElement(std::make_unique<ui::Button>(
          "Exit", font, [this]() { scene_manager_.PopScene(); }))
      ->size.y = kLabelSizeY;
  menu_layout
      .AddElement(std::make_unique<ui::Button>(
          "Play", font,
          [this]() {
            scene_manager_.PopScene();
            scene_manager_.PushScene(
                std::make_unique<GameScene>(scene_manager_));
            if (game::save_data.completion_markers.empty()) {
              scene_manager_.PushScene(
                  std::make_unique<IntroScene>(scene_manager_));
            }
          }))
      ->size.y = kLabelSizeY;
  menu_layout.AddElement(std::make_unique<ui::Label>("Dying Anima", font))
      ->size.y = kLabelSizeY * 2;
  menu_layout.SetSpacing(kLabelSizeY);
}

void MenuScene::Update(double /* dt */) {
  if (core::input::IsKeyPressedThisFrame(GLFW_KEY_ESCAPE)) {
    scene_manager_.PopScene();
  }
  if (core::input::IsKeyPressedThisFrame(GLFW_KEY_ENTER)) {
    switch (static_cast<AppState>(focus_index)) {
      case AppState::kPlaying:
        scene_manager_.PopScene();
        scene_manager_.PushScene(std::make_unique<GameScene>(scene_manager_));
        if (game::save_data.completion_markers.empty()) {
          scene_manager_.PushScene(
              std::make_unique<IntroScene>(scene_manager_));
        }
        break;
      case AppState::kExit:
        glfwSetWindowShouldClose(GetGameWindow().window, 1);
        break;
      case AppState::kLevelEditor:
        scene_manager_.PopScene();
        scene_manager_.PushScene(std::make_unique<LevelEditor>(scene_manager_));
        break;
      default:
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
  window.SetProjection(ProjectionType::kScreenSpace);
  GetCamera().SetType(CameraType::kUi);
  menu_layout.Render(text_shader, rect_shader);
}

void MenuScene::Quit() {
  glfwSwapInterval(0);  // Psycho mode
}
