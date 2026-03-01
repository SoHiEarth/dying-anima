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

#include "atlas.h"
#include "calculate.h"
#include "camera.h"
#include "core/input.h"
#include "core/resource_manager.h"
#include "font.h"
#include "game.h"
#include "game/pause.h"
#include "level_utils.h"
#include "physics.h"
#include "player.h"
#include "rect.h"
#include "render.h"
#include "saves.h"
#include "shader.h"
#include "sprite.h"
#include "state.h"
#include "transform.h"
#include "window.h"

void GameScene::Init() {
  physics::Init({0, -9.81f});
  registry = LoadLevel("level.txt");
  player = registry.create();
  auto &player_transform = registry.emplace<Transform>(player);
  registry.emplace<PlayerSpeed>(player, 2.0f, 4.0f, 0.0625f, 100.0f, 5.0f);
  auto &player_health = registry.emplace<Health>(player, 100.0f);
  player_body = registry
                    .emplace<PhysicsBody>(
                        player, physics::CreateBody(player_transform, true))
                    .body;
  registry.emplace<Sprite>(player, "game.player",
                           ResourceManager::GetTexture("game.player").texture);
  if (!std::filesystem::exists("saves") || std::filesystem::is_empty("saves")) {
    std::filesystem::create_directory("saves");
  } else {
    auto save_data = SaveManager::LoadLatestSave();
    player_transform = save_data.player_transform;
    player_health = save_data.player_health;
  }

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
  SaveManager::SaveGame(registry.get<Transform>(player),
                        registry.get<Health>(player));
  physics::Quit();
  render::Clear();
  registry.clear();
}

void GameScene::HandleInput() {
  auto &player_speed = registry.get<PlayerSpeed>(player);
  auto &player_body = registry.get<PhysicsBody>(player).body;

  if (core::input::IsKeyPressedThisFrame(GLFW_KEY_ESCAPE)) {
    scene_manager.PushScene(std::make_unique<PauseScene>(scene_manager));
  }

  b2Vec2 velocity = b2Body_GetLinearVelocity(player_body);
  if (core::input::IsKeyPressed(GLFW_KEY_A)) {
    if (velocity.x > -player_speed.max_speed && IsOnGround(player_body))
      velocity.x -= player_speed.speed;
  }
  if (core::input::IsKeyPressed(GLFW_KEY_D)) {
    if (velocity.x < player_speed.max_speed && IsOnGround(player_body))
      velocity.x += player_speed.speed;
  }
  if (!core::input::IsKeyPressed(GLFW_KEY_A) &&
      !core::input::IsKeyPressed(GLFW_KEY_D)) {
    velocity.x *= player_speed.deceleration;
  }
  b2Body_SetLinearVelocity(player_body, velocity);

  if (core::input::IsKeyPressed(GLFW_KEY_SPACE) && IsOnGround(player_body)) {
    b2Body_ApplyLinearImpulse(player_body,
                              b2Vec2(0.0f, player_speed.jump_impulse),
                              b2Body_GetLocalCenterOfMass(player_body), true);
  }

  if (core::input::IsKeyPressed(GLFW_KEY_A) &&
      core::input::IsKeyPressedThisFrame(GLFW_KEY_LEFT_SHIFT)) {
    b2Body_ApplyForceToCenter(player_body,
                              b2Vec2(-player_speed.boost_speed, 0.0f), true);
  }
  if (core::input::IsKeyPressed(GLFW_KEY_D) &&
      core::input::IsKeyPressedThisFrame(GLFW_KEY_LEFT_SHIFT)) {
    b2Body_ApplyForceToCenter(player_body,
                              b2Vec2(player_speed.boost_speed, 0.0f), true);
  }
}

void GameScene::Update(float dt) {
  float speed = 1.0f - std::pow(0.2f, (float)dt);
  auto &camera_position = GetCamera().position;
  auto &player_transform = registry.get<Transform>(player);
  camera_position = glm::mix({camera_position.x, camera_position.y, 0.0f},
                             glm::vec3(player_transform.position, 0.0f), speed);

  physics_accumulator += (float)dt;
  while (physics_accumulator >= physics_time_step) {
    b2World_Step(physics::world, 1.0f / 60.0f, 4);
    physics_accumulator -= physics_time_step;
  }
  auto physics_view = registry.view<Transform, PhysicsBody>();
  for (auto entity : physics_view) {
    physics::SyncPosition(physics_view.get<PhysicsBody>(entity).body,
                          physics_view.get<Transform>(entity).position);
  }
}

void GameScene::Render(GameWindow &window) {
  auto &camera_position = GetCamera().position;
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  window.SetProjection(ProjectionType::CENTERED);
  GetCamera().SetType(CameraType::WORLD);
  auto sprite_view = registry.view<Transform, Sprite>();
  for (auto entity : sprite_view) {
    sprite_view.get<Sprite>(entity).texture->Render(
        sprite_shader,
        CalculateModelMatrix(sprite_view.get<Transform>(entity)));
  }

  window.SetProjection(ProjectionType::SCREEN_SPACE);
  GetCamera().SetType(CameraType::UI);
  auto &player_health = registry.get<Health>(player);
  special_font->Render(
      std::format("Health: {}", static_cast<int>(player_health.health)),
      glm::vec2(20.0f, 20.0f), glm::vec3(1.0f), text_shader);

  // debug info: fps
  static double previous_seconds = glfwGetTime();
  static int frame_count = 0;
  double current_seconds = glfwGetTime();
  frame_count++;
  double fps =
      static_cast<double>(frame_count) / (current_seconds - previous_seconds);
  if (current_seconds - previous_seconds >= 1.0) {
    previous_seconds = current_seconds;
    frame_count = 0;
  }

  ImGui::Begin("Telemetry");
  ImGui::Text("FPS: %.2f", fps);
  auto &player_transform = registry.get<Transform>(player);
  if (ImGui::DragFloat3("Position",
                        glm::value_ptr(player_transform.position))) {
    b2Body_SetTransform(
        player_body,
        b2Vec2(player_transform.position.x, player_transform.position.y),
        b2Body_GetRotation(player_body));
    b2Body_SetLinearVelocity(player_body, b2Vec2(0.0f, 0.0f));
  }
  auto velocity = b2Body_GetLinearVelocity(player_body);
  ImGui::Text("Velocity: (%.2f, %.2f)", velocity.x, velocity.y);
  ImGui::Text("On Ground: %s", IsOnGround(player_body) ? "Yes" : "No");
  ImGui::Text("Camera Position: (%.2f, %.2f)", camera_position.x,
              camera_position.y);
  ImGui::End();

  ImGui::Begin("Player Tweaker");
  auto &player_speed = registry.get<PlayerSpeed>(player);
  ImGui::DragFloat("Max Speed", &player_speed.max_speed, 0.1f, 0.0f);
  ImGui::DragFloat("Speed", &player_speed.speed, 0.01f, 0.0f);
  ImGui::DragFloat("Deceleration", &player_speed.deceleration, 0.01f, 0.0f);
  ImGui::DragFloat("Boost Speed", &player_speed.boost_speed, 1.0f, 0.0f);
  ImGui::DragFloat("Jump Impulse", &player_speed.jump_impulse, 1.0f, 0.0f);
  ImGui::End();

  ImGui::Render();
  ImGui::EndFrame();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}