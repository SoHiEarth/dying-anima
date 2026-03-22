#include <glad/glad.h>
// code block
#include <GLFW/glfw3.h>
#include <box2d/box2d.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <entt/entt.hpp>
#include <filesystem>
#include <format>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <pugixml.hpp>

#include "core/atlas.h"
#include "core/camera.h"
#include "core/input.h"
#include "core/physics.h"
#include "core/render.h"
#include "core/resource_manager.h"
#include "core/transform.h"
#include "core/window.h"
#include "game/enemy.h"
#include "game/game.h"
#include "game/log.h"
#include "game/pause.h"
#include "game/player.h"
#include "level_utils.h"
#include "saves.h"
#include "sprite.h"
#include "util/calculate.h"
#include "core/animation.h"

namespace {
bool show_log = false;
bool show_player_info = false;
}

SaveData game::save_data{};

void GameScene::Init() {
  physics::Init({0, -39.81F});
  registry = LoadLevel("level.txt");
  player = registry.create();
  auto& player_transform = registry.emplace<Transform>(player);
  registry.emplace<PlayerSpeed>(player);
  auto& player_health = registry.emplace<Health>(player, 100.0F);
  player_body = registry
                    .emplace<PhysicsBody>(
                        player, physics::CreateBody(player_transform, true))
                    .body;
  registry.emplace<Sprite>(player, "game.player",
                           ResourceManager::GetTexture("game.player").texture);
  if (!std::filesystem::exists("saves") || std::filesystem::is_empty("saves")) {
    std::filesystem::create_directory("saves");
  } else if (!game::save_data.valid) {
    game::save_data = SaveManager::LoadLatestSave();
    player_transform = game::save_data.player_transform;
    player_health = game::save_data.player_health;
  }
  player_log.LoadLog();

  sprite_shader = ResourceManager::GetShader("Sprite").shader;
  rect_shader = ResourceManager::GetShader("Rect").shader;
  text_shader = ResourceManager::GetShader("Text").shader;
  special_font = ResourceManager::GetFont("Special").font;
  title_font = ResourceManager::GetFont("Title").font;
  ui_font = ResourceManager::GetFont("UI").font;
  sprite_shader->Use();
  sprite_shader->SetUniform("texture1", 0);
  text_shader->Use();
  text_shader->SetUniform("character", 0);
  glfwSwapInterval(0);
}

void GameScene::Quit() {
  if (!std::filesystem::exists("saves") || std::filesystem::is_empty("saves")) {
    std::filesystem::create_directory("saves");
  }
  player_log.SaveLog();
  SaveData save_data{};
  if (!std::filesystem::is_empty("saves")) {
    save_data = SaveManager::LoadLatestSave();
  }
  save_data.player_transform = registry.get<Transform>(player);
  save_data.player_health = registry.get<Health>(player);
  save_data.completion_markers = game::save_data.completion_markers;
  SaveManager::SaveGame(save_data);
  physics::Quit();
  registry.clear();
}

void GameScene::Update(double dt) {
  auto& player_speed = registry.get<PlayerSpeed>(player);
  player_body = registry.get<PhysicsBody>(player).body;

  if (core::input::IsKeyPressedThisFrame(GLFW_KEY_ESCAPE)) {
    scene_manager.PushScene(std::make_unique<PauseScene>(scene_manager));
  }

  b2Vec2 velocity = b2Body_GetLinearVelocity(player_body);
  float speed_multiplier = 1.0F;
  if (IsOnGround(player_body)) {
    speed_multiplier = 1.0F;
  } else {
    speed_multiplier =
        player_speed.air_control_multiplier;  // Air control is reduced
  }

  if (core::input::IsKeyPressed(GLFW_KEY_A)) {
    if (velocity.x > 0) {
      velocity.x *= player_speed.deceleration;
    }
    velocity.x = std::max(velocity.x - (player_speed.speed * speed_multiplier),
                          -player_speed.max_speed);
  }
  if (core::input::IsKeyPressed(GLFW_KEY_D)) {
    if (velocity.x < 0) {
      velocity.x *= player_speed.deceleration;
    }
    velocity.x = std::min(velocity.x + (player_speed.speed * speed_multiplier),
                          player_speed.max_speed);
  }
  if (!core::input::IsKeyPressed(GLFW_KEY_A) &&
      !core::input::IsKeyPressed(GLFW_KEY_D)) {
    velocity.x *= player_speed.deceleration;
  }

  if (core::input::IsKeyPressed(GLFW_KEY_LEFT_SHIFT)) {
    if (velocity.x > 0)
      velocity.x = std::min(velocity.x * player_speed.boost_speed,
                            player_speed.max_boost_speed);
    else if (velocity.x < 0)
      velocity.x = std::max(velocity.x * player_speed.boost_speed,
                            -player_speed.max_boost_speed);
  }

  if (velocity.x > 0) {
    auto& transform = registry.get<Transform>(player);
    transform.scale.x = 1.0f;
  } else if (velocity.x < 0) {
    auto& transform = registry.get<Transform>(player);
    transform.scale.x = -1.0f;  // Flip sprite when moving left
  }
  b2Body_SetLinearVelocity(player_body, velocity);

  if (core::input::IsKeyPressed(GLFW_KEY_SPACE) && IsOnGround(player_body)) {
    b2Body_ApplyLinearImpulse(player_body,
                              b2Vec2(0.0F, player_speed.jump_impulse),
                              b2Body_GetLocalCenterOfMass(player_body), true);
  }

  // Frame-independent camera smoothing
  constexpr float kCameraSmoothing = 5.0F;  // Smoothing factor
  float speed = std::min(
      1.0F, 1.0F - std::exp(-kCameraSmoothing * static_cast<float>(dt)));
  auto& camera_position = GetCamera().position;
  auto& player_transform = registry.get<Transform>(player);
  player_transform.z_index = 2.0F;
  camera_position = glm::mix({camera_position.x, camera_position.y, 0.0F},
                             glm::vec3(player_transform.position, 0.0F), speed);

  physics_accumulator += static_cast<float>(dt);
  while (physics_accumulator >= physics_time_step) {
    b2World_Step(physics::world, physics_time_step, 4);
    physics_accumulator -= physics_time_step;
  }
  auto physics_view = registry.view<Transform, PhysicsBody>();
  physics_view.each(
      [](auto /* entity */, Transform& transform, PhysicsBody& physicsBody) {
        physics::SyncPosition(physicsBody.body, transform.position);
      });
  game::UpdatePlayerDamagers(registry, static_cast<float>(dt));
  UpdateAnimations(registry, static_cast<float>(dt));

  if (core::input::IsKeyPressedThisFrame(GLFW_KEY_TAB)) {
    show_log = !show_log;
  }

  if (core::input::IsKeyPressedThisFrame(GLFW_KEY_P)) {
    show_player_info = !show_player_info;
  }
}

void GameScene::Render(GameWindow& window) {
  auto& camera_position = GetCamera().position;

  window.SetProjection(ProjectionType::CENTERED);
  GetCamera().SetType(CameraType::kWorld);
  render::Render(registry);

  window.SetProjection(ProjectionType::SCREEN_SPACE);
  GetCamera().SetType(CameraType::kUi);
  auto& player_health = registry.get<Health>(player);
  auto health_full_count = static_cast<int>(player_health.health / 10.0f);
  auto health_partial_count =
      (player_health.health - (health_full_count * 10.0)) / 10.0f;
  auto health_empty_count =
      10 - health_full_count - (health_partial_count > 0 ? 1 : 0);
  int health_x = 20;
  const int health_y = window.height - 40;
  for (int i = 0; i < health_full_count; i++) {
    ResourceManager::GetTexture("game.ui.heart_full")
        .texture->Render(sprite_shader,
                         CalculateModelMatrix({health_x += 50, health_y}, 0.0f,
                                              0.0f, {40.0f, 40.0f}));
  }
  for (int i = 0; i < health_partial_count; i++) {
    ResourceManager::GetTexture("game.ui.heart_half")
        .texture->Render(sprite_shader,
                         CalculateModelMatrix({health_x += 50, health_y}, 0.0f,
                                              0.0f,
                                              {40.0f, 40.0f}));
  }
  for (int i = 0; i < health_empty_count; i++) {
    ResourceManager::GetTexture("game.ui.heart_empty")
        .texture->Render(sprite_shader,
                         CalculateModelMatrix({health_x += 50, health_y}, 0.0f,
                                              0.0f,
                                              {40.0f, 40.0f}));
  }

  if (show_log) {
    player_log.RenderLog(game::LEVEL_0);
  }

  if (show_player_info) {
    game::RenderPlayerInfo();
  }
}
