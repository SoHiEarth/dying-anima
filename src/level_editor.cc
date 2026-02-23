#include <glad/glad.h>
// Code block
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "atlas.h"
#include "camera.h"
#include "core/input.h"
#include "core/resource_manager.h"
#include "level_editor.h"
#include "level_utils.h"
#include "object.h"
#include "rect.h"
#include "shader.h"
#include "window.h"
#include <entt/entt.hpp>
#include <print>

glm::dvec2 mouse_position, last_mouse_position;

void RenderGrid(GameWindow& window, const Shader* shader) {
  auto camera_position = GetCamera().position;
  float grid_size = window.GetPixelsPerUnit();
  glm::vec2 window_size = {window.width, window.height};
  glm::vec2 world_window_size = window_size / grid_size;
  glm::vec2 grid_origin = camera_position - world_window_size / 2.0f;
  glm::vec2 grid_end = camera_position + world_window_size / 2.0f;
  for (float x = std::floor(grid_origin.x); x <= grid_end.x; x += 1.0f) {
    Rect rect{};
    rect.position = {x, grid_origin.y};
    rect.scale = {0.01f, world_window_size.y};
    rect.color = {0.5f, 0.5f, 0.5f, 1.0f};
    rect.Render(shader);
  }
  for (float y = std::floor(grid_origin.y); y <= grid_end.y; y += 1.0f) {
    Rect rect{};
    rect.position = {grid_origin.x, y};
    rect.scale = {world_window_size.x, 0.01f};
    rect.color = {0.5f, 0.5f, 0.5f, 1.0f};
    rect.Render(shader);
  }
}

void LevelEditor::Init() {
  rect_shader = ResourceManager::GetShader("Rect").shader;
}

void LevelEditor::Quit() {}

void LevelEditor::HandleInput() {}

void LevelEditor::Update(float dt) {}

void LevelEditor::Render(GameWindow& window) {
  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  window.SetProjection(ProjectionType::CENTERED);
  GetCamera().SetType(CameraType::WORLD);
  RenderGrid(window, rect_shader);
}
