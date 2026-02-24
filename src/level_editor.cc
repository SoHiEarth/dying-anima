#include <glad/glad.h>
// Code block
#include <imgui.h>
#include <imgui_stdlib.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "window.h"
#include "camera.h"
#include "rect.h"
#include "shader.h"
#include "transform.h"
#include "sprite.h"
#include "level_editor.h"
#include "level_utils.h"
#include "core/resource_manager.h"
#include "core/input.h"
#include "calculate.h"

entt::entity selected_entity = entt::null;
glm::dvec2 mouse_position, last_mouse_position;
glm::vec2 ScreenToWorld(const glm::vec2& screen_pos, const Camera& camera,
                        const GameWindow& window) {
  float ppu = window.GetPixelsPerUnit();
  int width = window.width;
  int height = window.height;
  glm::vec2 centered_screen = screen_pos - glm::vec2(width / 2.0f, height / 2.0f);
  centered_screen.y = -centered_screen.y;
  glm::vec2 world_pos = centered_screen / ppu + camera.position;
  return world_pos;
}

void MouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
  GetGameWindow().SetPixelsPerUnit(GetGameWindow().GetPixelsPerUnit() +
                                   (float)yoffset);
  if (GetGameWindow().GetPixelsPerUnit() < 10.0f) {
    GetGameWindow().SetPixelsPerUnit(10.0f);
  }
   if (GetGameWindow().GetPixelsPerUnit() > 500.0f) {
    GetGameWindow().SetPixelsPerUnit(500.0f);
   }
}

void LevelEditor::Init() {
  registry = LoadLevel("level.txt");
  rect_shader = ResourceManager::GetShader("Rect").shader;
  sprite_shader = ResourceManager::GetShader("Sprite").shader;
  glfwSetScrollCallback(GetGameWindow().window, MouseScrollCallback);
}

void LevelEditor::Quit() {
  SaveLevel("level.txt", registry);
}

bool PointInRect(const glm::vec2& point, const Transform& transform) {
  glm::vec2 half_scale = transform.scale * 0.5f;
  glm::vec2 min = transform.position - half_scale;
  glm::vec2 max = transform.position + half_scale;
  return (point.x >= min.x && point.x <= max.x &&
          point.y >= min.y && point.y <= max.y);
}

bool ObjectExistsAtPosition(const glm::vec2& position, entt::registry& registry) {
  auto view = registry.view<Transform>();
  for (auto entity : view) {
    const auto& transform = view.get<Transform>(entity);
    if (PointInRect(position, transform)) {
      return true;
    }
  }
  return false;
}

entt::entity GetEntityAtPosition(const glm::vec2& position, entt::registry& registry) {
  auto view = registry.view<Transform>();
  for (auto entity : view) {
    const auto& transform = view.get<Transform>(entity);
    if (PointInRect(position, transform)) {
      return entity;
    }
  }
  return entt::null;
}

void LevelEditor::HandleInput() {
  last_mouse_position = mouse_position;
  glfwGetCursorPos(GetGameWindow().window, &mouse_position.x,
                   &mouse_position.y);
  glm::vec2 world_pos =
      ScreenToWorld(mouse_position, GetCamera(), GetGameWindow());
  world_pos.x = std::floor(world_pos.x) + 0.5f;
  world_pos.y = std::floor(world_pos.y) + 0.5f;
  if (core::input::IsKeyPressed(GLFW_MOUSE_BUTTON_MIDDLE)) {
    GetCamera().position.x += (last_mouse_position.x - mouse_position.x) /
                              GetGameWindow().GetPixelsPerUnit();
    GetCamera().position.y -= (last_mouse_position.y - mouse_position.y) /
                              GetGameWindow().GetPixelsPerUnit();
  }
  if (core::input::IsKeyPressed(GLFW_KEY_LEFT_CONTROL)) {
    if (core::input::IsKeyPressed(GLFW_MOUSE_BUTTON_LEFT)) {
      if (!ObjectExistsAtPosition(world_pos, registry)) {
        auto entity = registry.create();
        auto& transform = registry.emplace<Transform>(entity);
        transform.position = world_pos;
        transform.scale = glm::vec2(1.0f);
        transform.rotation = 0.0f;
        auto& sprite = registry.emplace<Sprite>(entity);
        sprite.texture_tag = "util.notexture";
        sprite.texture =
            ResourceManager::GetTexture(sprite.texture_tag).texture;
      } else {
        selected_entity = GetEntityAtPosition(world_pos, registry);
      }
    } else if (core::input::IsKeyPressed(GLFW_MOUSE_BUTTON_RIGHT)) {
      auto view = registry.view<Transform>();
      for (auto entity : view) {
        const auto& transform = view.get<Transform>(entity);
        if (PointInRect(world_pos, transform)) {
          registry.destroy(entity);
          break;
        }
      }
    }
  }
}

void LevelEditor::Update(float dt) {

}

void LevelEditor::Render(GameWindow& window) {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  window.SetProjection(ProjectionType::CENTERED);
  GetCamera().SetType(CameraType::WORLD);

  auto sprite_view = registry.view<Transform, Sprite>();
  for (entt::entity entity : sprite_view) {
    sprite_view.get<Sprite>(entity).texture->Render(
        sprite_shader,
        CalculateModelMatrix(sprite_view.get<Transform>(entity)));
  }

  auto& camera = GetCamera();
  float gridSize = 1.0f;
  float lineThickness = 0.01f;
  float halfWidth = (window.width / window.GetPixelsPerUnit()) / 2.0f;
  float halfHeight = (window.height / window.GetPixelsPerUnit()) / 2.0f;
  float left = camera.position.x - halfWidth;
  float right = camera.position.x + halfWidth;
  float bottom = camera.position.y - halfHeight;
  float top = camera.position.y + halfHeight;
  float startX = floor(left / gridSize) * gridSize;
  float startY = floor(bottom / gridSize) * gridSize;

  // Vertical lines
  for (float x = startX; x <= right; x += gridSize) {
    Rect line(glm::vec2(x, camera.position.y),
              glm::vec2(lineThickness, halfHeight * 2.0f),
              glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));
    line.Render(rect_shader);
  }

  // Horizontal lines
  for (float y = startY; y <= top; y += gridSize) {
    Rect line(glm::vec2(camera.position.x, y),
              glm::vec2(halfWidth * 2.0f, lineThickness),
              glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));
    line.Render(rect_shader);
  }

  // Origin lines
  Rect xAxis(glm::vec2(0.0f, camera.position.y),
             glm::vec2(lineThickness * 2.0f, halfHeight * 2.0f),
             glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
  xAxis.Render(rect_shader);
  Rect yAxis(glm::vec2(camera.position.x, 0.0f),
             glm::vec2(halfWidth * 2.0f, lineThickness * 2.0f),
             glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
  yAxis.Render(rect_shader);

  {
    ImGui::Begin("Render Info");
    ImGui::Text("Position: (%.2f, %.2f)", camera.position.x, camera.position.y);
    ImGui::Text("Window PPU: %.2f", window.GetPixelsPerUnit());
    ImGui::Text("Camera Type: %s", camera.GetType() == CameraType::WORLD
                                       ? "WORLD"
                                       : "SCREEN_SPACE");
    ImGui::End();
  }
  {
    ImGui::Begin("Selected Entity");
    if (registry.try_get<Transform>(selected_entity) == nullptr) {
      selected_entity = entt::null;
    }
    if (selected_entity != entt::null) {
      auto& transform = registry.get<Transform>(selected_entity);
      ImGui::DragFloat2("Position", glm::value_ptr(transform.position), 0.1f);
      ImGui::DragFloat2("Scale", glm::value_ptr(transform.scale), 0.1f);
      ImGui::DragFloat("Rotation", &transform.rotation, 1.0f);
      auto& sprite = registry.get<Sprite>(selected_entity);
      if (ImGui::InputText("Texture Tag", &sprite.texture_tag)) {
        sprite.texture =
            ResourceManager::GetTexture(sprite.texture_tag).texture;
      }
    } else {
      ImGui::Text("No entity selected");
    }
    ImGui::End();
  }

  ImGui::Render();
  ImGui::EndFrame();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
