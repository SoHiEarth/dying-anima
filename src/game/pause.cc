#include "game/pause.h"

#include "core/camera.h"
#include "core/input.h"
#include "core/rect.h"
#include "core/resource_manager.h"
#include "level_editor.h"
#include "menu.h"
#include "ui/elements.h"
namespace {
std::unique_ptr<ui::VerticalLayout> layout;
glm::ivec2 GetMousePos() {
  double x;
  double y;
  glfwGetCursorPos(glfwGetCurrentContext(), &x, &y);
  return glm::ivec2(x, y);
}
}  // namespace

void PauseScene::Init() {
  rect_shader = ResourceManager::GetShader("Rect").shader;
  text_shader = ResourceManager::GetShader("Text").shader;
  title_font = ResourceManager::GetFont("Title").font;
  ui_font = ResourceManager::GetFont("UI").font;
  glfwSwapInterval(1);  // Save resources
  layout = std::make_unique<ui::VerticalLayout>();
  layout->SetSpacing(20);
  layout->SetPadding(20, 20, 20, 20);
#ifndef NDEBUG
  layout->AddElement(
      std::make_unique<ui::Button>("Level Editor", ui_font, [this]() {
        scene_manager.PopScene();
        scene_manager.PopScene();  // Pop the game scene
        scene_manager.PushScene(std::make_unique<LevelEditor>(scene_manager));
      }));
#endif
  layout->AddElement(std::make_unique<ui::Button>("Exit", ui_font, []() {
    glfwSetWindowShouldClose(glfwGetCurrentContext(), true);
  }));
  layout->AddElement(std::make_unique<ui::Button>("Menu", ui_font, [this]() {
    scene_manager.PopScene();
    scene_manager.PopScene();  // Pop the game scene
    scene_manager.PushScene(std::make_unique<MenuScene>(scene_manager));
  }));
  layout->AddElement(std::make_unique<ui::Button>(
      "Resume", ui_font, [this]() { scene_manager.PopScene(); }));
  layout->AddElement(std::make_unique<ui::Label>("PAUSED", title_font));
}

void PauseScene::Quit() {
  glfwSwapInterval(0);  // Psycho mode
}

void PauseScene::Update(double dt) {
  layout->SetPosition(
      {30, (GetGameWindow().height - layout->GetLayoutSize()) / 2});
  time_since_open += dt;
  if (core::input::IsKeyPressedThisFrame(GLFW_KEY_ESCAPE)) {
    scene_manager.PopScene();
  }
  auto mouse_pos = GetMousePos();
  layout->Update(mouse_pos, core::input::IsKeyPressed(GLFW_MOUSE_BUTTON_LEFT));
}

void PauseScene::Render(GameWindow& window) {
  window.SetProjection(ProjectionType::SCREEN_SPACE);
  GetCamera().SetType(CameraType::kUi);
  Rect pause_rect;
  pause_rect.position = glm::vec2(window.width / 2.0F, window.height / 2.0F);
  pause_rect.scale = {window.width, window.height};
  pause_rect.color = glm::vec4(0.0F, 0.0F, 0.0F, 0.5F);
  pause_rect.Render(rect_shader);

  Rect pause_rect_2;
  pause_rect_2.position = glm::vec2(window.width / 6.0F, window.height / 2.0F);
  pause_rect_2.scale = {window.width / 3.0F, window.height};
  pause_rect_2.color = glm::vec4(0.0F, 0.0F, 0.0F, 0.7F);
  pause_rect_2.Render(rect_shader);
  layout->Render(text_shader, rect_shader);
}
