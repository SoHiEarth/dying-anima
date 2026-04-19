#include <glad/glad.h>
// code block
#include <GLFW/glfw3.h>
#include <box2d/box2d.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <tinyfiledialogs.h>

#include <entt/entt.hpp>
#include <filesystem>
#include "core/log.h"
#include "game/spawn.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <pugixml.hpp>

#include "core/animation.h"
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

SaveData game::save_data;
entt::registry game::registry;
game::Log game::player_log;
entt::entity game::player;

namespace {
bool show_log = false;
bool show_player_info = false;

enum class GetPlayerSpawnError { kNoSpawnPoint };
std::expected<Transform, GetPlayerSpawnError> GetPlayerSpawn() {
  auto spawn_view = game::registry.view<Transform, PlayerSpawn>();
  auto spawn_entity = spawn_view.front();
  if (spawn_entity != entt::null) {
    auto transform = game::registry.get<Transform>(spawn_entity);
    core::Log("Player spawn point found at (" + std::to_string(transform.position.x) + ", " +
                  std::to_string(transform.position.y) + ")",
              "Game");
    return transform;
  }
  core::Log("No player spawn point found in the level.", "Game");
  return std::unexpected(GetPlayerSpawnError::kNoSpawnPoint);
}

void DeleteDefeatedEnemies(entt::registry& registry) {
  auto view = registry.view<BattleTrigger>();
  for (auto entity : view) {
    auto& trigger = view.get<BattleTrigger>(entity);
    for (auto& enemy : trigger.enemies) {
      for (auto& uid : game::save_data.defeated_enemy_uids) {
        if (enemy.uid == uid) {
          registry.destroy(entity);
          break;
        }
      }
    }
  }
}
}

void GameScene::Init() {
  GetGameWindow().SetWindowSizeType(WindowSizeType::kFramebufferSize);
  physics::Init({0, -35.0F});
  game::registry = LoadLevel("level.txt");
  game::player = game::registry.create();
  auto& player_transform = game::registry.emplace<Transform>(game::player);
  auto player_spawn = GetPlayerSpawn();
  if (player_spawn.has_value()) {
    player_transform = player_spawn.value();
  }
  try {
    if (std::filesystem::exists(save_data_path)) {
      auto returned = save_manager::LoadGame(save_data_path.generic_string());
      if (returned.has_value()) {
        game::save_data = returned.value();
      }
    }
    else {
      auto returned = save_manager::LoadLatestSave();
      if (returned.has_value()) {
        game::save_data = returned.value();
      }
    }
  }
  catch (std::exception& e) {
    core::Log("Caught exception: " + std::string(e.what()) + " while loading save.", "Game");
  }
  game::registry.emplace<PlayerSpeed>(game::player);
  auto& player_skills = game::registry.emplace<PlayerSkills>(game::player);
  auto& player_health = game::registry.emplace<Health>(game::player, 100.0F);
  player_body = game::registry
                    .emplace<PhysicsBody>(game::player,
                                physics::CreateBody(player_transform, true, 0.0F))
                    .body;
  game::registry.emplace<Sprite>(
      game::player, "game.player",
                           resource_manager::GetTexture("game.player").texture);
  player_transform = game::save_data.player_transform;
  player_health = game::save_data.player_health;
  game::player_log = game::save_data.log;
  player_skills.skills = game::save_data.acquired_skills;
  if (player_skills.skills.empty()) {
    player_skills.skills.push_back({
        .name = "Punch",
        .damage = 50.0F,
        .stamina_drain = 5.0F,
        .health_used = 5.0F,
        .stamina_used = 0.0F,
    });
  }
  DeleteDefeatedEnemies(game::registry);
  sprite_shader = resource_manager::GetShader("Sprite").shader;
  rect_shader = resource_manager::GetShader("Rect").shader;
  text_shader = resource_manager::GetShader("Text").shader;
  special_font = resource_manager::GetFont("Special").font;
  title_font = resource_manager::GetFont("Title").font;
  ui_font = resource_manager::GetFont("UI").font;
  sprite_shader->Use();
  sprite_shader->SetUniform("texture1", 0);
  text_shader->Use();
  text_shader->SetUniform("character", 0);
  GetGameWindow().SetPixelsPerUnit(150.0F);
  glfwSwapInterval(0);
}

void GameScene::Quit() {
  SaveData save_data{};
  if (!std::filesystem::exists("saves")) {
    std::filesystem::create_directory("saves");
  } else if (!std::filesystem::is_empty("saves")){
    save_data = save_manager::LoadLatestSave().value();
  }
  save_data.player_transform = game::registry.get<Transform>(game::player);
  save_data.player_health = game::registry.get<Health>(game::player);
  save_data.completion_markers = game::save_data.completion_markers;
  save_manager::SaveGame(save_data, game::player_log);
  physics::Quit();
  game::registry.clear();
}

void GameScene::Update(double dt) {
  auto& player_speed = game::registry.get<PlayerSpeed>(game::player);
  player_body = game::registry.get<PhysicsBody>(game::player).body;

  if (core::input::IsKeyPressedThisFrame(GLFW_KEY_ESCAPE)) {
    scene_manager_.PushScene(std::make_unique<PauseScene>(scene_manager_));
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
    auto& transform = game::registry.get<Transform>(game::player);
    transform.scale.x = 1.0F;
  } else if (velocity.x < 0) {
    auto& transform = game::registry.get<Transform>(game::player);
    transform.scale.x = -1.0F;  // Flip sprite when moving left
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
  auto& player_transform = game::registry.get<Transform>(game::player);
  player_transform.z_index = 2.0F;
  camera_position = glm::mix({camera_position.x, camera_position.y, 0.0F},
                             glm::vec3(player_transform.position, 0.0F), speed);

  physics_accumulator += static_cast<float>(dt);
  while (physics_accumulator >= physics_time_step) {
    b2World_Step(physics::world, physics_time_step, 4);
    physics_accumulator -= physics_time_step;
  }
  auto physics_view = game::registry.view<Transform, PhysicsBody>();
  physics_view.each(
      [](auto /* entity */, Transform& transform, PhysicsBody& physicsBody) {
        physics::SyncTransform(physicsBody.body, transform);
      });
  game::UpdatePathFinders(game::registry);
  game::UpdateBattleTriggers(game::registry, scene_manager_);
  UpdateAnimations(game::registry, static_cast<float>(dt));

  if (core::input::IsKeyPressedThisFrame(GLFW_KEY_TAB)) {
    show_log = !show_log;
  }

  if (core::input::IsKeyPressedThisFrame(GLFW_KEY_P)) {
    show_player_info = !show_player_info;
  }
}

void GameScene::Render(GameWindow& window) {
  window.SetProjection(ProjectionType::kCentered);
  GetCamera().SetType(CameraType::kWorld);
  render::Render(game::registry);

  window.SetProjection(ProjectionType::kScreenSpace);
  GetCamera().SetType(CameraType::kUi);
  auto& player_health = game::registry.get<Health>(game::player);
  auto health_full_count = static_cast<int>(player_health.health / 10.0F);
  auto health_partial_count =
      (player_health.health - (health_full_count * 10.0)) / 10.0F;
  auto health_empty_count =
      10 - health_full_count - (health_partial_count > 0 ? 1 : 0);
  int health_x = 20;
  const int health_y = window.height - 40;
  for (int i = 0; i < health_full_count; i++) {
    resource_manager::GetTexture("game.ui.heart_full")
        .texture->Render(sprite_shader,
                         CalculateModelMatrix({health_x += 50, health_y}, 0.0F,
                                              0.0F, {40.0F, 40.0F}));
  }
  for (int i = 0; i < health_partial_count; i++) {
    resource_manager::GetTexture("game.ui.heart_half")
        .texture->Render(sprite_shader,
                         CalculateModelMatrix({health_x += 50, health_y}, 0.0F,
                                              0.0F, {40.0F, 40.0F}));
  }
  for (int i = 0; i < health_empty_count; i++) {
    resource_manager::GetTexture("game.ui.heart_empty")
        .texture->Render(sprite_shader,
                         CalculateModelMatrix({health_x += 50, health_y}, 0.0F,
                                              0.0F, {40.0F, 40.0F}));
  }

  if (show_log) {
    game::player_log.RenderLog(game::kLevel0);
  }

  if (show_player_info) {
    game::RenderPlayerInfo();
  }
}
