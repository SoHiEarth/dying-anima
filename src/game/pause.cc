#include "game/pause.h"

#include "camera.h"
#include "core/input.h"
#include "core/resource_manager.h"
#include "rect.h"
#include "menu.h"
#include "level_editor.h"

double anim_full_time = 0.4f;
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
  int padding = 10;
  if (mouse_pos.x >= x_pos &&
      mouse_pos.x <= x_pos + title_font->GetWidth("PAUSED") &&
      mouse_pos.y >= y_pos - title_font->GetHeight("PAUSED") &&
      mouse_pos.y <= y_pos) {
    if (core::input::IsKeyPressedThisFrame(GLFW_MOUSE_BUTTON_LEFT)) {
      scene_manager.PopScene();
    }
  }
  y_pos -= title_font->GetHeight("PAUSED") + padding;
  if (mouse_pos.x >= x_pos &&
      mouse_pos.x <= x_pos + ui_font->GetWidth("Resume") &&
      mouse_pos.y >= y_pos - ui_font->GetHeight("Resume") &&
      mouse_pos.y <= y_pos) {
    if (core::input::IsKeyPressedThisFrame(GLFW_MOUSE_BUTTON_LEFT)) {
      scene_manager.PopScene();
    }
  }
  y_pos -= ui_font->GetHeight("Resume") + padding;
  if (mouse_pos.x >= x_pos &&
      mouse_pos.x <= x_pos + ui_font->GetWidth("Menu") &&
      mouse_pos.y >= y_pos - ui_font->GetHeight("Menu") &&
      mouse_pos.y <= y_pos) {
    if (core::input::IsKeyPressedThisFrame(GLFW_MOUSE_BUTTON_LEFT)) {
      scene_manager.PopScene();
      scene_manager.PushScene(std::make_unique<MenuScene>(scene_manager));
    }
  }
  y_pos -= ui_font->GetHeight("Menu") + padding;
  if (mouse_pos.x >= x_pos &&
      mouse_pos.x <= x_pos + ui_font->GetWidth("Exit") &&
      mouse_pos.y >= y_pos - ui_font->GetHeight("Exit") &&
      mouse_pos.y <= y_pos) {
    if (core::input::IsKeyPressedThisFrame(GLFW_MOUSE_BUTTON_LEFT)) {
      glfwSetWindowShouldClose(glfwGetCurrentContext(), true);
    }
  }
#ifndef NDEBUG
  y_pos -= ui_font->GetHeight("Exit") + padding;
  if (mouse_pos.x >= x_pos &&
      mouse_pos.x <= x_pos + ui_font->GetWidth("Level Editor") &&
      mouse_pos.y >= y_pos - ui_font->GetHeight("Level Editor") &&
      mouse_pos.y <= y_pos) {
    if (core::input::IsKeyPressedThisFrame(GLFW_MOUSE_BUTTON_LEFT)) {
      scene_manager.PopScene();
      scene_manager.PushScene(std::make_unique<LevelEditor>(scene_manager));
    }
  }
#endif
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
  int padding = 10;
  title_font->RenderUI("PAUSED", glm::vec2(x_pos, y_pos), glm::vec2(1.0f),
                       glm::vec3(1.0f),
                       text_shader);
  y_pos -= title_font->GetHeight("PAUSED") + padding;
  constexpr float scale = 0.75f;
  ui_font->RenderUI("Resume", glm::vec2(x_pos, y_pos), glm::vec2(scale), 
                  glm::vec3(1.0f),
                  text_shader);
  y_pos -= ui_font->GetHeight("Resume") + padding;
  ui_font->RenderUI("Menu", glm::vec2(x_pos, y_pos), glm::vec2(scale),
                  glm::vec3(1.0f),
                  text_shader);
  y_pos -= ui_font->GetHeight("Menu") + padding;
  ui_font->RenderUI("Exit", glm::vec2(x_pos, y_pos), glm::vec2(scale),
                  glm::vec3(1.0f),
                  text_shader);
#ifndef NDEBUG
  y_pos -= ui_font->GetHeight("Exit") + padding;
  ui_font->RenderUI("Level Editor", glm::vec2(x_pos, y_pos), glm::vec2(scale),
                  glm::vec3(1.0f),
                  text_shader);
#endif
}