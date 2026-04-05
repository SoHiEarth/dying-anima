#include <glad/glad.h>
// Code block
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <imgui_stdlib.h>

#include <algorithm>
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
#include "editor/tooltip.h"
#include "editor/onboarding.h"
#include "game/enemy.h"
#include "game/game.h"
#include "game/spawn.h"
#include "level_editor.h"
#include "level_utils.h"
#include "menu.h"
#include "sprite.h"
#include "tinyfiledialogs/tinyfiledialogs.h"
#include "util/calculate.h"

namespace {
enum class Toolkit { kSelect, kMove };
Toolkit current_tool = Toolkit::kSelect;
entt::entity selected_entity = entt::null;
glm::dvec2 mouse_position, last_mouse_position;
std::string current_scene_path;
entt::entity spotlight = entt::null;
bool enable_spotlight = true;
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

  for (int x = start_x; x <= right; x++) {
    Rect line(glm::vec2(x, camera.position.y),
              glm::vec2(line_thickness, window.height),
              glm::vec4(0.3F, 0.3F, 0.3F, 1.0F));
    line.Render(shader);
  }
  for (int y = start_y; y <= top; y++) {
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

std::once_flag register_components_flag;
}  // namespace

void LevelEditor::Init() {
  auto* level_path =
      tinyfd_openFileDialog("Open Scene", "", 0, nullptr, nullptr, 0);
  if (level_path) {
    current_scene_path = level_path;
    registry = LoadLevel(current_scene_path);
  }
  rect_shader = resource_manager::GetShader("Rect").shader;
  sprite_shader = resource_manager::GetShader("Sprite").shader;
  glfwSetScrollCallback(GetGameWindow().window, MouseScrollCallback);
  std::call_once(register_components_flag, []() {
    REGISTER_COMPONENT(Animation);
    REGISTER_COMPONENT(Transform);
    REGISTER_COMPONENT(Sprite);
    REGISTER_COMPONENT(Light);
    REGISTER_COMPONENT(PhysicsBody);
    REGISTER_COMPONENT(PlayerSpawn);
    REGISTER_COMPONENT(BattleTrigger);
  });
  GetGameWindow().SetPixelsPerUnit(100.0F);
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
    scene_manager_.PopScene();
    scene_manager_.PushScene(std::make_unique<MenuScene>(scene_manager_));
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
    light.type = LightType::kPoint;
    light.color = glm::vec3(1.0F, 1.0F, 1.0F);
    light.intensity = 1.0F;
    light.radial_falloff = 50.0F;
    light.volumetric_intensity = 0.05F;
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
              resource_manager::GetTexture(sprite.texture_tag).texture;
          sprite.normal =
              resource_manager::GetTexture(sprite.texture_tag).texture;
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
  window.SetProjection(ProjectionType::kCentered);
  GetCamera().SetType(CameraType::kWorld);
  render::Render(registry);

  auto& camera = GetCamera();
  if (enable_grid) {
    DrawGrid(window, camera, rect_shader);
  }

  auto* viewport = ImGui::GetMainViewport();
  ImGui::SetNextWindowPos(viewport->Pos);
  ImGui::SetNextWindowSize(viewport->Size);
  ImGui::SetNextWindowViewport(viewport->ID);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0F);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0F);
  auto window_flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
                      ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                      ImGuiWindowFlags_NoMove |
                      ImGuiWindowFlags_NoBringToFrontOnFocus |
                      ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_MenuBar |
                      ImGuiWindowFlags_NoBackground;
  ImGui::Begin("DockSpace", nullptr, window_flags);
  ImGui::PopStyleVar(2);
  ImGuiID dockspace_id = ImGui::GetID("DockSpace");
  ImGui::DockSpace(dockspace_id, ImVec2(0.0F, 0.0F),
                   ImGuiDockNodeFlags_PassthruCentralNode);
  ImGui::End();
  static bool first_dock_layout = true;
  if (first_dock_layout) {
    first_dock_layout = false;
    ImGui::DockBuilderRemoveNode(dockspace_id);
    ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
    ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->Size);
    ImGuiID dock_main_id = dockspace_id;
    ImGuiID dock_id_bottom = ImGui::DockBuilderSplitNode(
        dock_main_id, ImGuiDir_Down, 0.3F, nullptr, &dock_main_id);
    ImGuiID dock_id_left = ImGui::DockBuilderSplitNode(
        dock_main_id, ImGuiDir_Left, 0.25F, nullptr, &dock_main_id);
    ImGuiID dock_id_right = ImGui::DockBuilderSplitNode(
        dock_main_id, ImGuiDir_Right, 0.25F, nullptr, &dock_main_id);
    ImGui::DockBuilderDockWindow("Scene", dock_id_left);
    ImGui::DockBuilderDockWindow("Resource Manager", dock_id_bottom);
    ImGui::DockBuilderDockWindow("Utilities & Info", dock_id_left);
    ImGui::DockBuilderDockWindow("Entity Inspector", dock_id_right);
    ImGui::DockBuilderFinish(dockspace_id);
  }

  {
    if (ImGui::BeginMainMenuBar()) {
      if (ImGui::BeginMenu("File")) {
        if (ImGui::MenuItem("New", "Ctrl+N")) {
          registry.clear();
          current_scene_path.clear();
        }
        editor::SetTooltip("menu_bar.file.new");
        if (ImGui::MenuItem("Open", "Ctrl+O")) {
          auto* level_path =
              tinyfd_openFileDialog("Open Scene", "", 0, nullptr, nullptr, 0);
          if (level_path) {
            current_scene_path = level_path;
            registry = LoadLevel(current_scene_path);
          }
        }
        editor::SetTooltip("menu_bar.file.open");
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
        editor::SetTooltip("menu_bar.file.save");
        if (ImGui::MenuItem("Play This Scene")) {
          SaveLevel("level.txt", registry);
          scene_manager_.PopScene();
          scene_manager_.PushScene(std::make_unique<GameScene>(scene_manager_));
        }
        editor::SetTooltip("menu_bar.file.play");
        ImGui::EndMenu();
      }

      if (ImGui::BeginMenu("View")) {
        if (ImGui::MenuItem("Toggle Grid")) {
          enable_grid = !enable_grid;
        }
        if (ImGui::MenuItem("Toggle Spotlight")) {
          enable_spotlight = !enable_spotlight;
        }
        editor::SetTooltip("menu_bar.view.spotlight");
        if (ImGui::MenuItem("Animation Editor")) {
          editor::ShowAnimationWindow(!editor::internal::show_animation_window);
        }
        editor::SetTooltip("menu_bar.view.anim_editor");
        ImGui::EndMenu();
      }
      ImGui::EndMainMenuBar();
    }

    ImGui::Begin("Scene");
    if (ImGui::CollapsingHeader("Controls")) {
      ImGui::Text("Hold Middle Mouse Button to Pan");
      ImGui::Text("Hold Left Control + Left Mouse Button to Select/Place");
      ImGui::Text("Hold Left Control + Right Mouse Button to Delete");
    }
    // tool
    ImGui::SeparatorText("Toolkit");
    ImGui::RadioButton("Select", reinterpret_cast<int*>(&current_tool),
                       static_cast<int>(Toolkit::kSelect));
    editor::SetTooltip("scene.toolkit.select");
    ImGui::RadioButton("Move", reinterpret_cast<int*>(&current_tool),
                       static_cast<int>(Toolkit::kMove));
    editor::SetTooltip("scene.toolkit.move");
    ImGui::SeparatorText("Entities");
    auto transform_view = registry.view<const Transform>();
    int entity_count = 0;
    for (auto [entity, transform] : transform_view.each()) {
      if (entity == spotlight) {
        continue;
      }
      if (ImGui::Selectable(std::format("Entity {}", ++entity_count).c_str(),
                            selected_entity == entity)) {
        selected_entity = entity;
      }
    }
    ImGui::End();

    ImGui::Begin("Resource Manager");
    if (ImGui::Button("Reload Textures")) {
      resource_manager::ReloadTextures();
    }
    editor::SetTooltip("resource.reload_texture");
    float thumbnail_size = 64.0F;
    float padding = 16.0F;
    float cell_size = thumbnail_size + (padding * 2);
    float panel_width = ImGui::GetContentRegionAvail().x;
    int column_count = static_cast<int>(panel_width / cell_size);
    column_count = std::max(column_count, 1);
    int i = 0;
    for (const auto& [key, value] : resource_manager::texture_atlas) {
      ImGui::PushID(key.c_str());
      ImGui::BeginGroup();
      if (ImGui::ImageButton("", value.texture->id,
                             ImVec2(thumbnail_size, thumbnail_size),
                             IMGUI_TEXTURE_FLIP)) {
      }

      if (ImGui::BeginDragDropSource()) {
        ImGui::SetDragDropPayload("SPRITE_TEXTURE", key.c_str(),
                                  key.size() + 1);
        ImGui::Image(value.texture->id, ImVec2(32, 32), IMGUI_TEXTURE_FLIP);
        ImGui::EndDragDropSource();
      }

      ImGui::TextWrapped("%s", key.c_str());
      ImGui::EndGroup();
      if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Width: %dpx\nHeight: %dpx\nChannels: %d (%s)", value.texture->width, value.texture->height, value.texture->channels, (value.texture->channels == 3) ? "RGB" : "RGBA");
      }
      ImGui::PopID();
      i++;
      if (i % column_count != 0) {
        ImGui::SameLine();
      }
    }
    ImGui::End();
    ImGui::Begin("Utilities & Info");
    auto camera = GetCamera();
    ImGui::Text("Position: (%.2f, %.2f)", camera.position.x, camera.position.y);
    editor::SetTooltip("info.camera");
    ImGui::Text("Window PPU: %.2f", window.GetPixelsPerUnit());
    editor::SetTooltip("info.ppu");
    if (ImGui::CollapsingHeader("Lights")) {
      int i = 0;
      for (auto entity : registry.view<Light>()) {
        if (entity == spotlight) continue;
        i++;
        if (ImGui::Button(std::format("Select Light #{}", i).c_str())) {
          selected_entity = entity;
        }
      }
      if (i == 0) {
        ImGui::Text("No lights in scene.");
      }
    }
    editor::SetTooltip("info.lights");
    ImGui::SeparatorText("Options");
    ImGui::Checkbox("Enable Grid", &enable_grid);
    ImGui::Checkbox("Enable Spotlight", &enable_spotlight);
    if (ImGui::Button("Add Physics Bodies to All Transforms")) {
      for (auto entity : registry.view<Transform>()) {
        if (!registry.any_of<PhysicsBody>(entity)) {
          registry.emplace<PhysicsBody>(entity);
        }
      }
    }
    editor::SetTooltip("info.physics_body_shortcut");
    ImGui::End();
    ImGui::Begin("Entity Inspector");
    if (registry.try_get<Transform>(selected_entity) == nullptr) {
      selected_entity = entt::null;
    }
    if (selected_entity != entt::null) {
      if (ImGui::CollapsingHeader("Transform")) {
        auto& transform = registry.get<Transform>(selected_entity);
        ImGui::DragFloat2("Position", glm::value_ptr(transform.position), 0.1F);
        editor::SetTooltip("inspector.transform.position");
        ImGui::DragFloat("Z Index", &transform.z_index, 0.1F);
        editor::SetTooltip("inspector.transform.z_index");
        ImGui::DragFloat2("Scale", glm::value_ptr(transform.scale), 0.1F);
        editor::SetTooltip("inspector.transform.scale");
        ImGui::DragFloat("Rotation", &transform.rotation, 1.0F);
        editor::SetTooltip("inspector.transform.rotation");
      }
      if (registry.any_of<Sprite>(selected_entity)) {
        if (ImGui::CollapsingHeader("Sprite")) {
          auto& sprite = registry.get<Sprite>(selected_entity);
          ImGui::BeginGroup();
          if (sprite.texture) {
            ImGui::Image(sprite.texture->id,
                         ImVec2(thumbnail_size, thumbnail_size),
                         IMGUI_TEXTURE_FLIP);
          } else {
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "No Image Found.");
          }
          ImGui::Text("Tag: %s", sprite.texture_tag.c_str());
          ImGui::EndGroup();
          if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload* payload =
                    ImGui::AcceptDragDropPayload("SPRITE_TEXTURE")) {
              const char* new_tag = static_cast<const char*>(payload->Data);
              sprite.texture_tag = new_tag;
              sprite.texture =
                  resource_manager::GetTexture(sprite.texture_tag).texture;
            }
            ImGui::EndDragDropTarget();
          }
          if (ImGui::Button("Remove Sprite")) {
            registry.remove<Sprite>(selected_entity);
          }
        }
      }
      // Check if it has a physics body component
      if (registry.any_of<PhysicsBody>(selected_entity)) {
        if (ImGui::CollapsingHeader("Physics Body")) {
          auto& physics_body = registry.get<PhysicsBody>(selected_entity);
          ImGui::Checkbox("Is Dynamic", &physics_body.is_dynamic);
          editor::SetTooltip("inspector.physics_body.dynamic");
          ImGui::Checkbox("Is Chained", &physics_body.is_chained);
          editor::SetTooltip("inspector.physics_body.chained");
          if (ImGui::Button("Remove Physics Body")) {
            registry.remove<PhysicsBody>(selected_entity);
          }
        }
      }
      // Check if it has a light component
      if (registry.any_of<Light>(selected_entity)) {
        if (ImGui::CollapsingHeader("Light")) {
          auto& light = registry.get<Light>(selected_entity);
          ImGui::Combo("Light Type", reinterpret_cast<int*>(&light.type),
                       "DIRECTIONAL\0POINT\0");
          editor::SetTooltip("inspector.light.type");
          ImGui::ColorEdit3("Light Color", glm::value_ptr(light.color));
          editor::SetTooltip("inspector.light.color");
          ImGui::DragFloat("Light Intensity", &light.intensity, 0.1F, 0.0F);
          editor::SetTooltip("inspector.light.intensity");
          if (light.type == LightType::kPoint) {
            ImGui::DragFloat("Light Radial Falloff", &light.radial_falloff,
                             0.1F, 0.0F);
            editor::SetTooltip("inspector.light.radial_falloff");
            ImGui::DragFloat("Light Volumetric Intensity",
                             &light.volumetric_intensity, 0.1F, 0.0F);
            editor::SetTooltip("inspector.light.volumetric_intensity");
          }
          if (ImGui::Button("Remove Light")) {
            registry.remove<Light>(selected_entity);
          }
        }
      }

      if (registry.any_of<BattleTrigger>(selected_entity)) {
        if (ImGui::CollapsingHeader("Battle Trigger")) {
          auto& trigger = registry.get<BattleTrigger>(selected_entity);
          std::vector<Enemy> enemies_to_erase;
          ImGui::DragFloat("Hitbox Radius", &trigger.hitbox_radius, 0.1F, 0.0F);
          for (auto& enemy : trigger.enemies) {
            ImGui::Text("Enemy: %s", enemy.name.c_str());
            ImGui::SameLine();
            if (ImGui::Button(std::format("Remove##{}", enemy.name).c_str())) {
              enemies_to_erase.push_back(enemy);
            }
          }
          for (auto enemy : enemies_to_erase) {
            if (auto it = std::ranges::find_if(
                    trigger.enemies.begin(), trigger.enemies.end(),
                    [&](const Enemy& e) { return e.name == enemy.name; });
                it != trigger.enemies.end()) {
              trigger.enemies.erase(it);
            }
          }
          if (ImGui::Button("Add Enemy")) {
            ImGui::OpenPopup("add_enemy_popup");
          }
          if (ImGui::BeginPopup("add_enemy_popup")) {
            static std::string enemy_name;
            ImGui::InputText("Enemy Name", &enemy_name);
            if (ImGui::Button("Add##add_enemy")) {
              if (!enemy_name.empty()) {
                trigger.enemies.push_back(
                    game::CreateEnemyFromName(enemy_name));
                enemy_name.clear();
              }
              ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
          }
          if (ImGui::Button("Remove Battle Trigger")) {
            registry.remove<BattleTrigger>(selected_entity);
          }
        }
      }

      if (registry.any_of<PlayerSpawn>(selected_entity)) {
        if (ImGui::CollapsingHeader("Player Spawn")) {
          if (ImGui::Button("Remove PlayerSpawn Component")) {
            registry.remove<PlayerSpawn>(selected_entity);
          }
        }
      }

      if (registry.any_of<Animation>(selected_entity)) {
        if (ImGui::CollapsingHeader("Animation")) {
          ImGui::Text("View and edit this animation in the Animation Editor");
          if (ImGui::Button("Edit Animation")) {
            editor::ShowAnimationWindow(true);
          }
          if (ImGui::Button("Remove Animation")) {
            registry.remove<Animation>(selected_entity);
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
          std::ranges::transform(lower_name, lower_name.begin(), ::tolower);
          std::ranges::transform(lower_search, lower_search.begin(), ::tolower);
          if (!lower_search.empty() && !lower_name.contains(search)) {
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
  
  if (editor::internal::show_onboarding_window)
    editor::RenderOnboarding();
}
