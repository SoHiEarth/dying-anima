#include "game/pause.h"
#include "core/resource_manager.h"
#include "rect.h"
#include "core/input.h"
#include "camera.h"

void PauseScene::Init() {
  rect_shader = ResourceManager::GetShader("Rect").shader;
  text_shader = ResourceManager::GetShader("Text").shader;
  title_font = ResourceManager::GetFont("Title").font;
  ui_font = ResourceManager::GetFont("UI").font;
  glfwSwapInterval(1); // Save resources
}

void PauseScene::Quit() {
  glfwSwapInterval(0); // Psycho mode
}

void PauseScene::HandleInput() {
	if (core::input::IsKeyPressedThisFrame(GLFW_KEY_ESCAPE)) {
			scene_manager.PopScene();
		}
}

void PauseScene::Update(float dt) {
	// No update logic needed for the pause scene
}

void PauseScene::Render(GameWindow& window) {
  window.SetProjection(ProjectionType::SCREEN_SPACE);
  GetCamera().SetType(CameraType::UI);
  Rect pause_rect;
  pause_rect.position = glm::vec2(window.width / 2.0f, window.height / 2.0f);
  pause_rect.scale = {window.width, window.height};
  pause_rect.color = glm::vec4(0.0f, 0.0f, 0.0f, 0.5f);
  pause_rect.Render(rect_shader);

  int x_pos = 30;
  int y_pos = window.height / 2 + 100;
  int padding = 10;
  title_font->Render("PAUSED", glm::vec2(x_pos, y_pos), glm::vec3(1.0f),
                     text_shader);
  y_pos -= title_font->GetHeight("PAUSED") + padding;
  ui_font->Render("Resume", glm::vec2(x_pos, y_pos), glm::vec3(1.0f),
                  text_shader);
  y_pos -= ui_font->GetHeight("Resume") + padding;
  ui_font->Render("Menu", glm::vec2(x_pos, y_pos), glm::vec3(1.0f),
                  text_shader);
  y_pos -= ui_font->GetHeight("Menu") + padding;
  ui_font->Render("Exit", glm::vec2(x_pos, y_pos), glm::vec3(1.0f),
                  text_shader);
#ifndef NDEBUG
  y_pos -= ui_font->GetHeight("Exit") + padding;
  ui_font->Render("Level Editor", glm::vec2(x_pos, y_pos), glm::vec3(1.0f),
                  text_shader);
#endif
}