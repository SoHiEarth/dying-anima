#include <glad/glad.h>
// Code block
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

#include "atlas.h"
#include "camera.h"
#include "core/input.h"
#include "core/resource_manager.h"
#include "font.h"
#include "level_editor.h"
#include "level_utils.h"
#include "object.h"
#include "rect.h"
#include "shader.h"
#include "window.h"

int GRID_SIZE = 5;
static int selected_object_index = -1;
static std::vector<Object> editor_objects;
glm::dvec2 mouse_position, last_mouse_position;

void ScrollCallback(GLFWwindow * /* window */, double /* xoffset */,
                    double yoffset) {
  GRID_SIZE = std::max(5, GRID_SIZE + static_cast<int>(yoffset) * 5);
}

void HandleLevelEditorInput(GameWindow window, AppState &app_state) {
  core::input::NewFrame();
  core::input::UpdateKeyState(window.window, GLFW_KEY_ESCAPE);
  core::input::UpdateKeyState(window.window, GLFW_MOUSE_BUTTON_LEFT);
  core::input::UpdateKeyState(window.window, GLFW_MOUSE_BUTTON_MIDDLE);

  if (core::input::IsKeyPressed(GLFW_KEY_ESCAPE)) {
    app_state = AppState::PLAYING;
  }
  glfwGetCursorPos(window.window, &mouse_position.x, &mouse_position.y);
  auto &camera_position = GetCamera().position;
  glm::ivec2 mouse_world_position = glm::ivec2(
      static_cast<int>(mouse_position.x + camera_position.x),
      static_cast<int>(window.height - (mouse_position.y - camera_position.y)));
  glm::ivec2 grid_position = glm::ivec2(mouse_world_position.x / GRID_SIZE,
                                        mouse_world_position.y / GRID_SIZE);
  if (core::input::IsKeyPressed(GLFW_MOUSE_BUTTON_MIDDLE)) {
    camera_position += glm::vec2(
        static_cast<float>(last_mouse_position.x - mouse_position.x),
        -static_cast<float>(last_mouse_position.y - mouse_position.y));
  }
  if (core::input::IsKeyPressed(GLFW_MOUSE_BUTTON_LEFT)) {
    selected_object_index = -1;
    for (size_t i = 0; i < editor_objects.size(); ++i) {
      auto &obj = editor_objects[i];
      if (mouse_world_position.x >= obj.position.x &&
          mouse_world_position.x <= obj.position.x + obj.scale.x &&
          mouse_world_position.y >= obj.position.y &&
          mouse_world_position.y <= obj.position.y + obj.scale.y) {
        selected_object_index = i;
        break;
      }
    }
    if (selected_object_index == -1) {
      Object object{};
      object.position = glm::vec3(grid_position.x * GRID_SIZE,
                                  grid_position.y * GRID_SIZE, 1.0f);
      object.scale = glm::vec3(static_cast<float>(GRID_SIZE),
                               static_cast<float>(GRID_SIZE), 1.0f);
      editor_objects.emplace_back(object);
      SaveLevel("level.txt", editor_objects);
    }
  }
  last_mouse_position = mouse_position;
}

AppState LevelEditor(GameWindow &window) {
  auto rect_shader = ResourceManager::GetShader("Rect").shader,
       text_shader = ResourceManager::GetShader("Text").shader;
  text_shader->Use();
  text_shader->SetUniform("character", 0);
  auto font = ResourceManager::GetFont("UI").font;
  glfwSetScrollCallback(window.window, ScrollCallback);

  AppState app_state = AppState::LEVEL_EDITOR;
  while (!glfwWindowShouldClose(window.window) &&
         app_state == AppState::LEVEL_EDITOR) {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    window.SetProjection(ProjectionType::CENTERED);
    GetCamera().SetType(CameraType::WORLD);
    for (const auto &object : editor_objects) {
      Rect box;
      box.position = glm::vec2(object.position.x + GRID_SIZE / 2.0f,
                               object.position.y + GRID_SIZE / 2.0f);
      box.scale = glm::vec2(object.scale.x, object.scale.y);
      box.color = glm::vec4(1.0f);
      box.Render(rect_shader);
    }

    auto &camera_position = GetCamera().position;
    float left = camera_position.x;
    float right = camera_position.x + window.width;
    float bottom = camera_position.y;
    float top = camera_position.y + window.height;
    int startX = static_cast<int>(std::floor(left / GRID_SIZE)) * GRID_SIZE;
    int endX = static_cast<int>(std::ceil(right / GRID_SIZE)) * GRID_SIZE;
    int startY = static_cast<int>(std::floor(bottom / GRID_SIZE)) * GRID_SIZE;
    int endY = static_cast<int>(std::ceil(top / GRID_SIZE)) * GRID_SIZE;
    for (int x = startX; x <= endX; x += GRID_SIZE) {
      Rect grid_vertical;
      grid_vertical.position = glm::vec2(x, (startY + endY) * 0.5f);
      grid_vertical.scale = glm::vec2(1.0f, endY - startY);
      grid_vertical.color = glm::vec4(0.3f);
      grid_vertical.Render(rect_shader);
    }

    for (int y = startY; y <= endY; y += GRID_SIZE) {
      Rect grid_horizontal;
      grid_horizontal.position = glm::vec2((startX + endX) * 0.5f, y);
      grid_horizontal.scale = glm::vec2(endX - startX, 1.0f);
      grid_horizontal.color = glm::vec4(0.3f);
      grid_horizontal.Render(rect_shader);
    }

    // draw origin lines
    Rect origin_vertical, origin_horizontal;
    origin_vertical.position = glm::vec2(0.0f, (startY + endY) * 0.5f);
    origin_vertical.scale = glm::vec2(2.0f, endY - startY);
    origin_vertical.color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    origin_vertical.Render(rect_shader);
    origin_horizontal.position = glm::vec2((startX + endX) * 0.5f, 0.0f);
    origin_horizontal.scale = glm::vec2(endX - startX, 2.0f);
    origin_horizontal.color = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
    origin_horizontal.Render(rect_shader);

    if (selected_object_index != -1) {
      auto &obj = editor_objects[selected_object_index];
      glm::vec2 center = glm::vec2(obj.position.x + obj.scale.x * 0.5f,
                                   obj.position.y + obj.scale.y * 0.5f);
      Rect x_axis, y_axis;
      x_axis.position = center + glm::vec2(20.0f, 0.0f);
      x_axis.scale = glm::vec2(40.0f, 4.0f);
      x_axis.color = glm::vec4(1, 0, 0, 1);
      x_axis.Render(rect_shader);
      y_axis.position = center + glm::vec2(0.0f, 20.0f);
      y_axis.scale = glm::vec2(4.0f, 40.0f);
      y_axis.color = glm::vec4(0, 1, 0, 1);
      y_axis.Render(rect_shader);
    }

    window.SetProjection(ProjectionType::SCREEN_SPACE);
    GetCamera().SetType(CameraType::UI);
    font->Render(std::format("Grid Size: {}", GRID_SIZE),
                 glm::vec2(10.0f, 10.0f), glm::vec3(1.0f), text_shader);
    glfwSwapBuffers(window.window);
    glfwPollEvents();
    HandleLevelEditorInput(window, app_state);
  }
  if (app_state == AppState::LEVEL_EDITOR) {
    app_state = AppState::EXIT;
  }
  return app_state;
}
