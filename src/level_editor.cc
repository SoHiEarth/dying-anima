#include <glad/glad.h>
// Code block
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_stdlib.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "core/animation.h"
#include "core/camera.h"
#include "core/component.h"
#include "core/input.h"
#include "core/light.h"
#include "core/physics.h"
#include "core/rect.h"
#include "core/render.h"
#include "core/resource_manager.h"
#include "core/shader.h"
#include "core/transform.h"
#include "core/window.h"
#include "editor/animation.h"
#include "game/enemy.h"
#include "game/spawn.h"
#include "level_editor.h"
#include "level_utils.h"
#include "menu.h"
#include "sprite.h"
#include "tinyfiledialogs/tinyfiledialogs.h"
#include "util/calculate.h"

enum class Toolkit { kSelect, kMove };
namespace {
Toolkit current_tool = Toolkit::kSelect;
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
      screen_pos - glm::vec2(width / 2.0F, height / 2.0F);
  centered_screen.y = -centered_screen.y;
  glm::vec2 world_pos = centered_screen / ppu + camera.position;
  return world_pos;
}

void MouseScrollCallback(GLFWwindow* /* window */, double /* xoffset */,
                         double yoffset) {
  if (!core::input::IsKeyPressed(GLFW_KEY_LEFT_CONTROL)) {
    return;
  }
  GetGameWindow().SetPixelsPerUnit(GetGameWindow().GetPixelsPerUnit() +
                                   static_cast<float>(yoffset));
  if (GetGameWindow().GetPixelsPerUnit() < 10.0F) {
    GetGameWindow().SetPixelsPerUnit(10.0F);
  }
  if (GetGameWindow().GetPixelsPerUnit() > 500.0F) {
    GetGameWindow().SetPixelsPerUnit(500.0F);
  }
}

bool PointInRect(const glm::vec2& point, const Transform& transform) {
  glm::vec2 half_scale = transform.scale * 0.5F;
  glm::vec2 min = transform.position - half_scale;
  glm::vec2 max = transform.position + half_scale;
  return (point.x >= min.x && point.x <= max.x && point.y >= min.y &&
          point.y <= max.y);
}

bool ObjectExistsAtPosition(const glm::vec2& position,
                            entt::registry& registry) {
  auto view = registry.view<const Transform>();
  return std::ranges::any_of(view.each(), [&](const auto& entity_transform) {
    auto [entity, transform] = entity_transform;
    if (entity == spotlight) {
      return false;
    }
    return PointInRect(position, transform);
  });
}

entt::entity GetEntityAtPosition(const glm::vec2& position,
                                 entt::registry& registry) {
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

void DrawGrid(GameWindow& window, Camera& camera,
              const std::shared_ptr<Shader>& shader) {
  float line_thickness = 1 / window.GetPixelsPerUnit();
  float half_width = (window.width / window.GetPixelsPerUnit()) / 2.0F;
  float half_height = (window.height / window.GetPixelsPerUnit()) / 2.0F;
  float left = camera.position.x - half_width;
  float right = camera.position.x + half_width;
  float bottom = camera.position.y - half_height;
  float top = camera.position.y + half_height;
  float start_x = floor(left);
  float start_y = floor(bottom);

  for (float x = start_x; x <= right; x++) {
    Rect line(glm::vec2(x, camera.position.y),
              glm::vec2(line_thickness, window.height),
              glm::vec4(0.3F, 0.3F, 0.3F, 1.0F));
    line.Render(shader);
  }
  for (float y = start_y; y <= top; y++) {
    Rect line(glm::vec2(camera.position.x, y),
              glm::vec2(window.width, line_thickness),
              glm::vec4(0.3F, 0.3F, 0.3F, 1.0F));
    line.Render(shader);
  }

  // Origin
  Rect x_axis(glm::vec2(0.0F, camera.position.y),
              glm::vec2(line_thickness * 2.0F, window.height),
              glm::vec4(1.0F, 0.0F, 0.0F, 1.0F));
  x_axis.Render(shader);
  Rect y_axis(glm::vec2(camera.position.x, 0.0F),
              glm::vec2(window.width, line_thickness * 2.0F),
              glm::vec4(0.0F, 1.0F, 0.0F, 1.0F));
  y_axis.Render(shader);
}
}  // namespace

void LevelEditor::Init() {
  auto* level_path =
      tinyfd_openFileDialog("Open Scene", "", 0, nullptr, nullptr, 0);
  if (level_path) {
    current_scene_path = level_path;
    registry = LoadLevel(current_scene_path);
  }
  rect_shader = ResourceManager::GetShader("Rect").shader;
  sprite_shader = ResourceManager::GetShader("Sprite").shader;
  glfwSetScrollCallback(GetGameWindow().window, MouseScrollCallback);
  REGISTER_COMPONENT(Animation);
  REGISTER_COMPONENT(Transform);
  REGISTER_COMPONENT(Sprite);
  REGISTER_COMPONENT(Light);
  REGISTER_COMPONENT(PhysicsBody);
  REGISTER_COMPONENT(PlayerSpawn);
  REGISTER_COMPONENT(PlayerDamager);
}

void LevelEditor::Quit() {
  glfwSetScrollCallback(GetGameWindow().window, nullptr);
  if (current_scene_path.empty()) {
    auto* level =
        tinyfd_saveFileDialog("Save Scene", "scene", 0, nullptr, nullptr);
    if (level) {
      current_scene_path = level;
      SaveLevel(current_scene_path, registry);
    }
  }
}

void LevelEditor::Update(double /* dt */) {
  if (core::input::IsKeyPressedThisFrame(GLFW_KEY_ESCAPE)) {
    // add menu
    scene_manager.PopScene();
    scene_manager.PushScene(std::make_unique<MenuScene>(scene_manager));
  }
  if (core::input::IsKeyPressed(GLFW_KEY_LEFT_CONTROL) &&
      core::input::IsKeyPressed(GLFW_KEY_S)) {
    if (current_scene_path.empty()) {
      auto* level =
          tinyfd_saveFileDialog("Save Scene", "scene", 0, nullptr, nullptr);
      if (level) {
        current_scene_path = level;
        SaveLevel(current_scene_path, registry);
      }
    } else {
      SaveLevel(current_scene_path, registry);
    }
  }
  if (enable_spotlight) {
    if (!registry.valid(spotlight)) {
      spotlight = registry.create();
    }
    if (!registry.any_of<Transform>(spotlight)) {
      registry.emplace<Transform>(spotlight);
    }
    if (!registry.any_of<Light>(spotlight)) {
      registry.emplace<Light>(spotlight);
    }
    auto& light = registry.get<Light>(spotlight);
    light.type = LightType::POINT;
    light.color = glm::vec3(1.0F, 1.0F, 1.0F);
    light.intensity = 1.0F;
    light.radial_falloff = 10.0F;
    light.volumetric_intensity = 0.25F;
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
  world_pos.x = std::floor(world_pos.x) + 0.5F;
  world_pos.y = std::floor(world_pos.y) + 0.5F;
  if (core::input::IsKeyPressed(GLFW_MOUSE_BUTTON_MIDDLE)) {
    GetCamera().position.x +=
        static_cast<float>(last_mouse_position.x - mouse_position.x) /
        GetGameWindow().GetPixelsPerUnit();
    GetCamera().position.y -=
        static_cast<float>(last_mouse_position.y - mouse_position.y) /
        GetGameWindow().GetPixelsPerUnit();
  }
  if (core::input::IsKeyPressed(GLFW_KEY_LEFT_CONTROL)) {
    if (core::input::IsKeyPressed(GLFW_MOUSE_BUTTON_LEFT)) {
      if (selected_entity != entt::null && current_tool == Toolkit::kMove) {
        registry.get<Transform>(selected_entity).position +=
            ScreenToWorld(mouse_position, GetCamera(), GetGameWindow()) -
            ScreenToWorld(last_mouse_position, GetCamera(), GetGameWindow());
      }
      if (current_tool == Toolkit::kSelect) {
        if (ObjectExistsAtPosition(world_pos, registry)) {
          selected_entity = GetEntityAtPosition(world_pos, registry);
        } else {
          auto entity = registry.create();
          auto& transform = registry.emplace<Transform>(entity);
          transform.position = world_pos;
          transform.scale = glm::vec2(1.0F);
          transform.rotation = 0.0F;
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
      auto entity = GetEntityAtPosition(world_pos, registry);
      if (entity != entt::null) {
        registry.destroy(entity);
        if (selected_entity == entity) {
          selected_entity = entt::null;
        }
      }
    }
  }
}

void LevelEditor::Render(GameWindow& window) {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  window.SetProjection(ProjectionType::CENTERED);
  GetCamera().SetType(CameraType::kWorld);
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
          auto* level_path =
              tinyfd_openFileDialog("Open Scene", "", 0, nullptr, nullptr, 0);
          if (level_path) {
            current_scene_path = level_path;
            registry = LoadLevel(current_scene_path);
          }
        }
        if (ImGui::MenuItem("Save", "Ctrl+S")) {
          if (current_scene_path.empty()) {
            auto* level = tinyfd_saveFileDialog("Save Scene", "scene", 0,
                                                nullptr, nullptr);
            if (level) {
              current_scene_path = level;
              SaveLevel(current_scene_path, registry);
            }
          } else {
            SaveLevel(current_scene_path, registry);
          }
        }
        ImGui::EndMenu();
      }

      if (ImGui::BeginMenu("View")) {
        if (ImGui::MenuItem("Toggle Grid")) {
          enable_grid = !enable_grid;
        }
        if (ImGui::MenuItem("Toggle Spotlight")) {
          enable_spotlight = !enable_spotlight;
        }
        if (ImGui::MenuItem("Animation Editor")) {
          editor::ShowAnimationWindow(!editor::internal::show_animation_window);
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
    ImGui::RadioButton("Select", reinterpret_cast<int*>(&current_tool),
                       static_cast<int>(Toolkit::kSelect));
    ImGui::RadioButton("Move", reinterpret_cast<int*>(&current_tool),
                       static_cast<int>(Toolkit::kMove));
    ImGui::SeparatorText("Resource Manager");
    ImGui::Text("Textures:");
    if (ImGui::Button("Reload Textures")) {
      ResourceManager::Quit();
      ResourceManager::Init();
    }
    for (const auto& [key, value] : ResourceManager::texture_atlas) {
      if (ImGui::CollapsingHeader(key.c_str())) {
        ImGui::Image(value.texture->id,
                     ImVec2(static_cast<float>(value.texture->width),
                            static_cast<float>(value.texture->height)));
        ImGui::Text("Dimensions: %dx%d", value.texture->width,
                    value.texture->height);
        ImGui::Text("File: %s", value.texture->path.c_str());
      }
    }
    ImGui::SeparatorText("Renderer");
    ImGui::Text("Position: (%.2f, %.2f)", camera.position.x, camera.position.y);
    ImGui::Text("Window PPU: %.2f", window.GetPixelsPerUnit());
    ImGui::Text("Camera Type: %s", camera.GetType() == CameraType::kWorld
                                       ? "WORLD"
                                       : "SCREEN_SPACE");
    if (ImGui::CollapsingHeader("Lights")) {
      int i = 0;
      for (auto entity : registry.view<Light>()) {
        i++;
        if (ImGui::Button(std::format("Select Light #{}", i).c_str())) {
          selected_entity = entity;
        }
      }
    }
    ImGui::SeparatorText("Options");
    ImGui::Checkbox("Enable Grid", &enable_grid);
    ImGui::Checkbox("Enable Spotlight", &enable_spotlight);
    if (ImGui::Button("Add Physics Bodies to All Transforms")) {
      for (auto entity : registry.view<Transform>()) {
        if (!registry.any_of<PhysicsBody>(entity)) {
          registry.emplace<PhysicsBody>(entity);
          // Do not add b2BodyId because b2World is not initialized
        }
      }
    }
    ImGui::SeparatorText("Entity Info");
    if (registry.try_get<Transform>(selected_entity) == nullptr) {
      selected_entity = entt::null;
    }
    if (selected_entity != entt::null) {
      if (ImGui::CollapsingHeader("Transform")) {
        auto& transform = registry.get<Transform>(selected_entity);
        ImGui::DragFloat2("Position", glm::value_ptr(transform.position), 0.1F);
        ImGui::DragFloat("Z Index", &transform.z_index, 0.1F);
        ImGui::DragFloat2("Scale", glm::value_ptr(transform.scale), 0.1F);
        ImGui::DragFloat("Rotation", &transform.rotation, 1.0F);
      }
      if (ImGui::CollapsingHeader("Sprite")) {
        if (registry.any_of<Sprite>(selected_entity)) {
          auto& sprite = registry.get<Sprite>(selected_entity);
          if (ImGui::InputText("Color Tag", &sprite.texture_tag)) {
            sprite.texture =
                ResourceManager::GetTexture(sprite.texture_tag).texture;
          }
          static std::string normal_texture_tag = sprite.texture_tag;
          if (ImGui::InputText("Normal Map Tag", &normal_texture_tag)) {
            sprite.normal =
                ResourceManager::GetTexture(normal_texture_tag).texture;
          }
        }
      }
      // Check if it has a physics body component
      if (registry.any_of<PhysicsBody>(selected_entity)) {
        if (ImGui::CollapsingHeader("Physics Body")) {
          auto& physics_body = registry.get<PhysicsBody>(selected_entity);
          ImGui::Checkbox("Is Dynamic", &physics_body.is_dynamic);
          ImGui::Checkbox("Is Chained", &physics_body.is_chained);
        }
      }
      // Check if it has a light component
      if (registry.any_of<Light>(selected_entity)) {
        if (ImGui::CollapsingHeader("Light")) {
          auto& light = registry.get<Light>(selected_entity);
          ImGui::Combo("Light Type", reinterpret_cast<int*>(&light.type),
                       "DIRECTIONAL\0POINT\0");
          ImGui::ColorEdit3("Light Color", glm::value_ptr(light.color));
          ImGui::DragFloat("Light Intensity", &light.intensity, 0.1F, 0.0F);
          if (light.type == LightType::POINT) {
            ImGui::DragFloat("Light Radial Falloff", &light.radial_falloff,
                             0.1F, 0.0F);
            ImGui::DragFloat("Light Volumetric Intensity",
                             &light.volumetric_intensity, 0.1F, 0.0F);
          }
        }
      }

      if (registry.any_of<PlayerDamager>(selected_entity)) {
        if (ImGui::CollapsingHeader("Player Damager")) {
          auto& damager = registry.get<PlayerDamager>(selected_entity);
          ImGui::DragFloat("Damage", &damager.damage, 1.0F, 0);
          ImGui::DragFloat("Hitbox Radius", &damager.hitbox_radius, 0.1F, 0.0F);
          ImGui::DragFloat("Knockback", &damager.knockback, 0.1F, 0.0F);
          ImGui::DragFloat2("Knockback Direction",
                            glm::value_ptr(damager.knockback_direction), 0.1F);
          ImGui::DragFloat("Time Until Next Hit", &damager.time_until_next_hit,
                           0.1F, 0.0F);
        }
      }

      if (registry.any_of<PlayerSpawn>(selected_entity)) {
        if (ImGui::Button("Remove PlayerSpawn Component")) {
          registry.remove<PlayerSpawn>(selected_entity);
        }
      }

      if (registry.any_of<Animation>(selected_entity)) {
        if (ImGui::CollapsingHeader("Animation")) {
          ImGui::Text("View and edit this animation in the Animation Editor");
          if (ImGui::Button("Edit Animation")) {
            editor::ShowAnimationWindow(true);
          }
        }
      }

      ImGui::SeparatorText("Utilities");

      if (ImGui::Button("Add Component")) {
        ImGui::OpenPopup("add_component_popup");
      }

      static std::string search;
      if (ImGui::BeginPopup("add_component_popup")) {
        if (ImGui::IsWindowAppearing()) {
          search.clear();
        }
        ImGui::InputText("Search", &search);
        for (const auto& component : GetComponentRegistry()) {
          if (component.Has(registry, selected_entity)) {
            continue;
          }

          std::string lower_name = component.name;
          std::string lower_search = search;
          std::transform(lower_name.begin(), lower_name.end(),
                         lower_name.begin(), ::tolower);
          std::transform(lower_search.begin(), lower_search.end(),
                         lower_search.begin(), ::tolower);
          if (!lower_search.empty() &&
              lower_name.find(search) == std::string::npos) {
            continue;
          }

          if (ImGui::Selectable(component.name.c_str())) {
            component.Add(registry, selected_entity);
            ImGui::CloseCurrentPopup();
          }
        }
        ImGui::EndPopup();
      }

      if (ImGui::Button("Delete Entity")) {
        ImGui::OpenPopup("delete_entity_popup");
      }
      if (ImGui::BeginPopup("delete_entity_popup")) {
        ImGui::Text("Are you sure you want to delete this entity?");
        if (ImGui::Button("Yes")) {
          registry.destroy(selected_entity);
          selected_entity = entt::null;
          ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel")) {
          ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
      }
    } else {
      ImGui::Text("No entity selected");
    }

    ImGui::End();
  }

  if (editor::internal::show_animation_window) {
    editor::AnimationWindow(registry.get<Animation>(selected_entity));
  }

  ImGui::Render();
  ImGui::EndFrame();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
