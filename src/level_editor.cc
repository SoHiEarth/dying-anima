#include <glad/glad.h>
// Code block
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_stdlib.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "calculate.h"
#include "camera.h"
#include "core/input.h"
#include "core/resource_manager.h"
#include "level_editor.h"
#include "level_utils.h"
#include "light.h"
#include "rect.h"
#include "render.h"
#include "shader.h"
#include "sprite.h"
#include "tinyfiledialogs/tinyfiledialogs.h"
#include "transform.h"
#include "window.h"

enum class Toolkit { SELECT, MOVE };

Toolkit current_tool = Toolkit::SELECT;
entt::entity selected_entity = entt::null;
glm::dvec2 mouse_position, last_mouse_position;
std::string current_scene_path;
entt::entity spotlight = entt::null;
bool enable_spotlight = false;
bool enable_grid = true;

glm::vec2 ScreenToWorld(const glm::vec2& screen_pos, const Camera& camera,
                        const GameWindow& window) {
  float ppu = window.GetPixelsPerUnit();
  int width = window.width;
  int height = window.height;
  glm::vec2 centered_screen =
      screen_pos - glm::vec2(width / 2.0f, height / 2.0f);
  centered_screen.y = -centered_screen.y;
  glm::vec2 world_pos = centered_screen / ppu + camera.position;
  return world_pos;
}

void MouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
  if (!core::input::IsKeyPressed(GLFW_KEY_LEFT_CONTROL)) {
    return;
  }
  GetGameWindow().SetPixelsPerUnit(GetGameWindow().GetPixelsPerUnit() +
                                   (float)yoffset);
  if (GetGameWindow().GetPixelsPerUnit() < 10.0f) {
    GetGameWindow().SetPixelsPerUnit(10.0f);
  }
  if (GetGameWindow().GetPixelsPerUnit() > 500.0f) {
    GetGameWindow().SetPixelsPerUnit(500.0f);
  }
}

bool PointInRect(const glm::vec2& point, const Transform& transform) {
  glm::vec2 half_scale = transform.scale * 0.5f;
  glm::vec2 min = transform.position - half_scale;
  glm::vec2 max = transform.position + half_scale;
  return (point.x >= min.x && point.x <= max.x && point.y >= min.y &&
          point.y <= max.y);
}

bool ObjectExistsAtPosition(const glm::vec2& position,
                            entt::registry& registry) {
  auto view = registry.view<Transform>();
  for (auto entity : view) {
    if (entity != spotlight) {
      const auto& transform = view.get<Transform>(entity);
      if (PointInRect(position, transform)) {
        return true;
      }
    }
  }
  return false;
}

entt::entity GetEntityAtPosition(const glm::vec2& position,
                                 entt::registry& registry,
                                 entt::entity spotlight) {
  auto view = registry.view<Transform>();
  for (auto entity : view) {
    if (entity == spotlight) continue;
    const auto& transform = view.get<Transform>(entity);
    if (PointInRect(position, transform)) {
      return entity;
    }
  }
  return entt::null;
}

void DrawGrid(GameWindow& window, Camera& camera, Shader* shader) {
  float lineThickness = 1 / window.GetPixelsPerUnit();
  float halfWidth = (window.width / window.GetPixelsPerUnit()) / 2.0f;
  float halfHeight = (window.height / window.GetPixelsPerUnit()) / 2.0f;
  float left = camera.position.x - halfWidth;
  float right = camera.position.x + halfWidth;
  float bottom = camera.position.y - halfHeight;
  float top = camera.position.y + halfHeight;
  float startX = floor(left);
  float startY = floor(bottom);

  for (float x = startX; x <= right; x++) {
    Rect line(glm::vec2(x, camera.position.y),
              glm::vec2(lineThickness, window.height),
              glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));
    line.Render(shader);
  }
  for (float y = startY; y <= top; y++) {
    Rect line(glm::vec2(camera.position.x, y),
              glm::vec2(window.width, lineThickness),
              glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));
    line.Render(shader);
  }

  // Origin
  Rect xAxis(glm::vec2(0.0f, camera.position.y),
             glm::vec2(lineThickness * 2.0f, window.height),
             glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
  xAxis.Render(shader);
  Rect yAxis(glm::vec2(camera.position.x, 0.0f),
             glm::vec2(window.width, lineThickness * 2.0f),
             glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
  yAxis.Render(shader);
}

void LevelEditor::Init() {
  auto level_path =
      tinyfd_openFileDialog("Open Scene", "", 0, nullptr, nullptr, 0);
  if (level_path) {
    current_scene_path = level_path;
    registry = LoadLevel(current_scene_path);
  }
  rect_shader = ResourceManager::GetShader("Rect").shader;
  sprite_shader = ResourceManager::GetShader("Sprite").shader;
  glfwSetScrollCallback(GetGameWindow().window, MouseScrollCallback);
}

void LevelEditor::Quit() {
  if (current_scene_path.empty()) {
    auto level =
        tinyfd_saveFileDialog("Save Scene", "scene", 0, nullptr, nullptr);
    if (level) {
      current_scene_path = level;
      SaveLevel(current_scene_path, registry);
    }
  }
  render::Clear();
}

void LevelEditor::HandleInput() {
  if (enable_spotlight) {
    if (!registry.valid(spotlight)) {
      spotlight = registry.create();
    }
    if (!registry.any_of<Transform>(spotlight)) {
      auto& transform = registry.emplace<Transform>(spotlight);
    }
    if (!registry.any_of<Light>(spotlight)) {
      registry.emplace<Light>(spotlight);
    }
    auto& light = registry.get<Light>(spotlight);
    light.type = LightType::POINT;
    light.color = glm::vec3(1.0f, 1.0f, 1.0f);
    light.intensity = 1.0f;
    light.radial_falloff = 10.0f;
    light.volumetric_intensity = 0.25f;
  } else {
    if (registry.valid(spotlight)) {
      registry.destroy(spotlight);
    }
  }
  last_mouse_position = mouse_position;
  glfwGetCursorPos(GetGameWindow().window, &mouse_position.x,
                   &mouse_position.y);
  glm::vec2 world_pos =
      ScreenToWorld(mouse_position, GetCamera(), GetGameWindow());
  if (enable_spotlight) registry.get<Transform>(spotlight).position = world_pos;
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
      if (selected_entity != entt::null && current_tool == Toolkit::MOVE) {
        registry.get<Transform>(selected_entity).position +=
            ScreenToWorld(mouse_position, GetCamera(), GetGameWindow()) -
            ScreenToWorld(last_mouse_position, GetCamera(), GetGameWindow());
      }
      if (current_tool == Toolkit::SELECT) {
        if (ObjectExistsAtPosition(world_pos, registry)) {
          selected_entity = GetEntityAtPosition(world_pos, registry, spotlight);
        } else {
          auto entity = registry.create();
          auto& transform = registry.emplace<Transform>(entity);
          transform.position = world_pos;
          transform.scale = glm::vec2(1.0f);
          transform.rotation = 0.0f;
          auto& sprite = registry.emplace<Sprite>(entity);
          sprite.texture_tag = "util.notexture";
          sprite.texture =
              ResourceManager::GetTexture(sprite.texture_tag).texture;
          sprite.normal =
              ResourceManager::GetTexture(sprite.texture_tag).texture;
          selected_entity = entity;
        }
      }
    } else if (core::input::IsKeyPressed(GLFW_MOUSE_BUTTON_RIGHT)) {
      auto entity = GetEntityAtPosition(world_pos, registry, spotlight);
      if (entity != entt::null) {
        registry.destroy(entity);
        if (selected_entity == entity) {
          selected_entity = entt::null;
        }
      }
    }
  }
}

void LevelEditor::Update(float dt) {}

void LevelEditor::Render(GameWindow& window) {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  window.SetProjection(ProjectionType::CENTERED);
  GetCamera().SetType(CameraType::WORLD);
  render::Render(registry);

  auto& camera = GetCamera();
  if (enable_grid) {
    DrawGrid(window, camera, rect_shader);
  }

  {
    if (ImGui::BeginMainMenuBar()) {
      if (ImGui::BeginMenu("File")) {
        if (ImGui::MenuItem("New", "Ctrl+N")) {
          registry.clear();
          current_scene_path.clear();
        }
        if (ImGui::MenuItem("Open", "Ctrl+O")) {
          auto level_path =
              tinyfd_openFileDialog("Open Scene", "", 0, nullptr, nullptr, 0);
          if (level_path) {
            current_scene_path = level_path;
            registry = LoadLevel(current_scene_path);
          }
        }
        if (ImGui::MenuItem("Save", "Ctrl+S")) {
          if (current_scene_path.empty()) {
            auto level = tinyfd_saveFileDialog("Save Scene", "scene", 0,
                                               nullptr, nullptr);
            if (level) {
              current_scene_path = level;
              SaveLevel(current_scene_path, registry);
            }
          }
        }
        ImGui::EndMenu();
      }
      ImGui::EndMainMenuBar();
    }
    ImGui::Begin("Info");
    if (ImGui::CollapsingHeader("Controls")) {
      ImGui::Text("Hold Middle Mouse Button to Pan");
      ImGui::Text("Hold Left Control + Left Mouse Button to Select/Place");
      ImGui::Text("Hold Left Control + Right Mouse Button to Delete");
    }
    // tool
    ImGui::SeparatorText("Toolkit");
    ImGui::RadioButton("Select", (int*)&current_tool, (int)Toolkit::SELECT);
    ImGui::RadioButton("Move", (int*)&current_tool, (int)Toolkit::MOVE);
    ImGui::SeparatorText("Renderer");
    ImGui::Text("Position: (%.2f, %.2f)", camera.position.x, camera.position.y);
    ImGui::Text("Window PPU: %.2f", window.GetPixelsPerUnit());
    ImGui::Text("Camera Type: %s", camera.GetType() == CameraType::WORLD
                                       ? "WORLD"
                                       : "SCREEN_SPACE");
    ImGui::SeparatorText("Options");
    ImGui::Checkbox("Enable Grid", &enable_grid);
    ImGui::Checkbox("Enable Spotlight", &enable_spotlight);
    ImGui::SeparatorText("Entity Info");
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
      // Check if it has a light component
      if (registry.any_of<Light>(selected_entity)) {
        if (ImGui::CollapsingHeader("Light")) {
          auto& light = registry.get<Light>(selected_entity);
          ImGui::Combo("Light Type", (int*)&light.type, "DIRECTIONAL\0POINT\0");
          ImGui::ColorEdit3("Light Color", glm::value_ptr(light.color));
          ImGui::DragFloat("Light Intensity", &light.intensity, 0.1f, 0.0f);
          if (light.type == LightType::POINT) {
            ImGui::DragFloat("Light Radial Falloff", &light.radial_falloff,
                             0.1f, 0.0f);
            ImGui::DragFloat("Light Volumetric Intensity",
                             &light.volumetric_intensity, 0.1f, 0.0f);
          }
        }
      } else {
        if (ImGui::Button("Add Light Component")) {
          registry.emplace<Light>(selected_entity);
        }
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
