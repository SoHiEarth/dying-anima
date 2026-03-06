#include "game/pause.h"

#include "core/camera.h"
#include "core/input.h"
#include "core/resource_manager.h"
#include "core/rect.h"
#include "menu.h"
#include "level_editor.h"
#include "ui/elements.h"

double anim_full_time = 0.4f;
std::unique_ptr<ui::VerticalLayout> layout;

glm::ivec2 GetMousePos() {
  double x, y;
  glfwGetCursorPos(glfwGetCurrentContext(), &x, &y);
  return glm::ivec2(x, y);
}

void PauseScene::Init() {
  rect_shader = ResourceManager::GetShader("Rect").shader;
  text_shader = ResourceManager::GetShader("Text").shader;
  title_font = ResourceManager::GetFont("Title").font;
  ui_font = ResourceManager::GetFont("UI").font;
  glfwSwapInterval(1);  // Save resources
  layout = std::make_unique<ui::VerticalLayout>();
  layout->SetSpacing(20);
  layout->SetPadding(20, 20, 20, 20);
  layout->AddElement(std::make_unique<ui::Label>("PAUSED", title_font));
  auto resume_button = layout->AddElement(std::make_unique<ui::Button>("Resume", ui_font, [this]() { scene_manager.PopScene(); }));
  auto menu_button = layout->AddElement(std::make_unique<ui::Button>("Menu", ui_font, [this]() {
    scene_manager.PopScene();
    scene_manager.PopScene();  // Pop the game scene
    scene_manager.PushScene(std::make_unique<MenuScene>(scene_manager));
  }));
  auto exit_button = layout->AddElement(std::make_unique<ui::Button>("Exit", ui_font, []() {
    glfwSetWindowShouldClose(glfwGetCurrentContext(), true);
  }));
#ifndef NDEBUG
  auto level_editor_button = layout->AddElement(std::make_unique<ui::Button>("Level Editor", ui_font, [this]() {
    scene_manager.PopScene();
    scene_manager.PopScene();  // Pop the game scene
    scene_manager.PushScene(std::make_unique<LevelEditor>(scene_manager));
      }));
#endif
  layout->UpdateLayout();
}

void PauseScene::Quit() {
  glfwSwapInterval(0);  // Psycho mode
}

void PauseScene::Update(double dt) {
  time_since_open += dt;
  if (core::input::IsKeyPressedThisFrame(GLFW_KEY_ESCAPE)) {
    scene_manager.PopScene();
  }
  auto mouse_pos = GetMousePos();
  int x_pos = 30 * (std::min(time_since_open, anim_full_time) / anim_full_time);
  int y_pos = GetGameWindow().height / 2 + 100;
  layout->Update(mouse_pos,
                 core::input::IsKeyPressed(GLFW_MOUSE_BUTTON_LEFT));
}

void PauseScene::Render(GameWindow& window) {
  window.SetProjection(ProjectionType::SCREEN_SPACE);
  GetCamera().SetType(CameraType::UI);
  Rect pause_rect;
  pause_rect.position = glm::vec2(window.width / 2.0f, window.height / 2.0f);
  pause_rect.scale = {window.width, window.height};
  pause_rect.color = glm::vec4(0.0f, 0.0f, 0.0f, 0.5f);
  pause_rect.Render(rect_shader);

  int x_pos = 30 * (std::min(time_since_open, anim_full_time) / anim_full_time);
  int y_pos = window.height / 2 + 100;
  layout->Render(text_shader, rect_shader);
}