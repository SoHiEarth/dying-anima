#include <glad/glad.h>
// code block
#include <GLFW/glfw3.h>
#include <box2d/box2d.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <algorithm>
#include <format>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <pugixml.hpp>
#include <entt/entt.hpp>

#include "atlas.h"
#include "calculate.h"
#include "core/input.h"
#include "core/quad.h"
#include "font.h"
#include "game.h"
#include "level_utils.h"
#include "physics.h"
#include "player.h"
#include "rect.h"
#include "saves.h"
#include "shader.h"
#include "state.h"
#include "window.h"
#include <filesystem>
#include "transform.h"
#include "sprite.h"

glm::vec3 camera_position{0.0f, 0.0f, 20.0f};
b2WorldId physics_world;

void HandleGameInput(GameState &game_state,
    entt::registry& registry,
    entt::entity player,
    GLFWwindow *window,
    double dt = 0.016) {
  static double time_since_no_input = 0;
  auto &player_speed = registry.get<PlayerSpeed>(player);
  auto &player_transform = registry.get<Transform>(player);
  auto &player_body = registry.get<PhysicsBody>(player).body;
  core::input::UpdateKeyState(window, GLFW_KEY_ESCAPE);
  core::input::UpdateKeyState(window, GLFW_KEY_A);
  core::input::UpdateKeyState(window, GLFW_KEY_D);
  core::input::UpdateKeyState(window, GLFW_KEY_SPACE);
  core::input::UpdateKeyState(window, GLFW_KEY_LEFT_SHIFT);

  if (core::input::IsKeyPressedThisFrame(GLFW_KEY_ESCAPE)) {
    if (game_state == GameState::PAUSED)
      game_state = GameState::RUNNING;
    else if (game_state == GameState::RUNNING)
      game_state = GameState::PAUSED;
  }
  if (game_state == GameState::PAUSED)
    return;

  b2Vec2 velocity = b2Body_GetLinearVelocity(player_body);
  if (core::input::IsKeyPressed(GLFW_KEY_A)) {
    if (velocity.x > -player_speed.max_speed && IsOnGround(player_body))
      velocity.x -= player_speed.speed;
  }
  if (core::input::IsKeyPressed(GLFW_KEY_D)) {
    if (velocity.x < player_speed.max_speed && IsOnGround(player_body))
      velocity.x += player_speed.speed;
  }
  if (!core::input::IsKeyPressed(GLFW_KEY_A) && !core::input::IsKeyPressed(GLFW_KEY_D)) {
    velocity.x *= player_speed.deceleration;
  }
  b2Body_SetLinearVelocity(player_body, velocity);

  if (core::input::IsKeyPressed(GLFW_KEY_SPACE) && IsOnGround(player_body)) {
    b2Body_ApplyLinearImpulse(player_body, b2Vec2(0.0f, player_speed.jump_impulse), b2Body_GetLocalCenterOfMass(player_body), true);
  }

  if (core::input::IsKeyPressed(GLFW_KEY_A) && core::input::IsKeyPressedThisFrame(GLFW_KEY_LEFT_SHIFT)) {
    b2Body_ApplyForceToCenter(player_body, b2Vec2(-player_speed.boost_speed, 0.0f), true);
  }
  if (core::input::IsKeyPressed(GLFW_KEY_D) && core::input::IsKeyPressedThisFrame(GLFW_KEY_LEFT_SHIFT)) {
    b2Body_ApplyForceToCenter(player_body, b2Vec2(player_speed.boost_speed, 0.0f), true);
  }

  if (std::abs(velocity.x) < 0.01f && std::abs(velocity.y) < 0.01f) {
    time_since_no_input += dt;
    if (time_since_no_input > 2.0f) {
      float speed = 1.0f - std::pow(0.2f, dt);
      camera_position = glm::mix(camera_position, glm::vec3(player_transform.position, 20.0f), speed);
    }
  } else {
    time_since_no_input = 0;
    camera_position.z = std::min(30.0f, camera_position.z * 1.002f);
  }
}

AppState Game(GameWindow window) {
  entt::registry registry;
  physics_world = physics::CreatePhysicsWorld({0.0f, -9.81f});
  auto shader_atlas = LoadShaderAtlas("assets/shaders/shader.xml");
  auto font_atlas = LoadFontAtlas("assets/fonts/font.xml");
  auto texture_atlas = LoadTextureAtlas("assets/textures/texture.xml");
  
  auto player = registry.create();
  auto& player_transform = registry.emplace<Transform>(player);
  registry.emplace<PlayerSpeed>(player, 2.0f, 4.0f, 0.0625, 100.0f, 5.0f);
  auto& player_health = registry.emplace<Health>(player, 100.0f);
  auto& player_body = registry.emplace<PhysicsBody>(player,
    physics::CreatePhysicsBody(
      physics_world, player_transform.position,
      player_transform.scale, player_transform.rotation, true)).body;
  registry.emplace<Sprite>(player, "game.player");
  
  LoadLevel("level.txt", registry, physics_world);
  if (!std::filesystem::exists("saves") || std::filesystem::is_empty("saves")) {
    std::filesystem::create_directory("saves");
  } else {
    auto save_data = SaveManager::LoadLatestSave();
    player_transform = save_data.player_transform;
    player_health = save_data.player_health;
  }

  Shader shader(shader_atlas.at("Sprite").vertex_file,
                shader_atlas.at("Sprite").fragment_file);
  shader.Use();
  shader.SetUniform("texture1", 0);

  Shader rect_shader(shader_atlas.at("Rect").vertex_file,
                     shader_atlas.at("Rect").fragment_file);
  Shader text_shader(shader_atlas.at("Text").vertex_file,
                     shader_atlas.at("Text").fragment_file);
  text_shader.Use();
  text_shader.SetUniform("character", 0);
  Font special_font(font_atlas.at("Special").file, 48);
  Font title_font(font_atlas.at("Title").file, 96);
  Font ui_font(font_atlas.at("UI").file, 32);

  AppState app_state = AppState::PLAYING;
  GameState game_state = GameState::RUNNING;
  while (!glfwWindowShouldClose(window.window) &&
         app_state == AppState::PLAYING) {
    static double previous_frame_time = glfwGetTime();
    double current_time = glfwGetTime();
    double frame_time = current_time - previous_frame_time;
    previous_frame_time = current_time;

    core::input::NewFrame();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    glm::vec3 screen_player_position = glm::vec3(-player_transform.position, 0.0f) + camera_position;
    if (std::abs(screen_player_position.x) > 5.0f) {
      camera_position.x = player_transform.position.x + 5.0f * glm::sign(screen_player_position.x);
    }
    if (std::abs(screen_player_position.y) > 5.0f) {
      camera_position.y = player_transform.position.y + 5.0f * glm::sign(screen_player_position.y);
    }

    glfwPollEvents();
    HandleGameInput(game_state, registry, player, window.window, frame_time);
    if (game_state == GameState::RUNNING) {
      b2World_Step(physics_world, 1.0f / 60.0f, 4);
    }
    auto view = glm::translate(glm::mat4(1.0f), -camera_position);
    float aspect = static_cast<float>(window.width) / static_cast<float>(window.height);
    auto projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    auto physics_view = registry.view<Transform, PhysicsBody>();
    for (auto entity : physics_view) {
      auto& transform = physics_view.get<Transform>(entity);
      auto& physics_body = physics_view.get<PhysicsBody>(entity);
      physics::SyncPosition(physics_body.body, transform.position);
    }
    auto sprite_view = registry.view<Transform, Sprite>();
    for (auto entity : sprite_view) {
      auto& transform = sprite_view.get<Transform>(entity);
      auto& sprite = sprite_view.get<Sprite>(entity);
      if (!texture_atlas.contains(sprite.texture_name)) {
        sprite.texture_name = "util.notexture";
      }
      texture_atlas.at(sprite.texture_name).texture->Render(
          shader, projection, view,
          CalculateModelMatrix(transform.position, transform.rotation, transform.scale));
      core::quad::Render(core::quad::QuadType::WITH_TEXCOORDS);
    }

    glm::mat4 ui_projection =
        glm::ortho(0.0f, static_cast<float>(window.width), 0.0f,
                   static_cast<float>(window.height));
    special_font.Render(
        std::format("Health: {}", static_cast<int>(player_health.health)),
        glm::vec2(20.0f, 20.0f), 1.0f, glm::vec3(1.0f), text_shader,
        ui_projection);

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

    ImGui::Begin("Debug Menu");
    ImGui::Text("FPS: %.2f", fps);
    ImGui::DragFloat3("Position", glm::value_ptr(player_transform.position));
    auto velocity = b2Body_GetLinearVelocity(player_body);
    ImGui::Text("Velocity: (%.2f, %.2f)", velocity.x, velocity.y);
    ImGui::Text("On Ground: %s", IsOnGround(player_body) ? "Yes" : "No");
    ImGui::Text("Camera Position: (%.2f, %.2f, %.2f)", camera_position.x,
                camera_position.y, camera_position.z);
    ImGui::Text("Game State: %s",
                game_state == GameState::RUNNING ? "Running" : "Paused");
    ImGui::End();

    ImGui::Begin("Texture Atlas");
    for (const auto &[name, entry] : texture_atlas) {
      ImGui::Text("Name: %s", name.c_str());
      ImGui::Image(entry.texture->id, ImVec2(100, 100));
    }
    ImGui::End();

    if (game_state == GameState::PAUSED) {
      Rect pause_rect;
      pause_rect.position =
          glm::vec2(window.width / 2.0f, window.height / 2.0f);
      pause_rect.scale = {window.width, window.height};
      pause_rect.color = glm::vec4(0.0f, 0.0f, 0.0f, 0.5f);
      pause_rect.Render(rect_shader, ui_projection, glm::mat4(1.0f));

      int x_pos = 30;
      int y_pos = window.height / 2 + 100;
      int padding = 10;
      title_font.Render("PAUSED", glm::vec2(x_pos, y_pos), 1.0f,
                        glm::vec3(1.0f), text_shader, ui_projection);
      y_pos -= title_font.GetHeight("PAUSED", 1.0f) + padding;
      ui_font.Render("Resume", glm::vec2(x_pos, y_pos), 1.0f, glm::vec3(1.0f),
                     text_shader, ui_projection);
      y_pos -= ui_font.GetHeight("Resume", 1.0f) + padding;
      ui_font.Render("Menu", glm::vec2(x_pos, y_pos), 1.0f, glm::vec3(1.0f),
                     text_shader, ui_projection);
      y_pos -= ui_font.GetHeight("Menu", 1.0f) + padding;
      ui_font.Render("Exit", glm::vec2(x_pos, y_pos), 1.0f, glm::vec3(1.0f),
                     text_shader, ui_projection);
#ifndef NDEBUG
      y_pos -= ui_font.GetHeight("Exit", 1.0f) + padding;
      ui_font.Render("Level Editor", glm::vec2(x_pos, y_pos), 1.0f,
                     glm::vec3(1.0f), text_shader, ui_projection);
#endif
    }

    ImGui::Render();
    ImGui::EndFrame();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window.window);
    core::input::UpdateLastFrameKeyStates();
  }
  SaveManager::SaveGame(player_transform, player_health);
  if (app_state == AppState::PLAYING) {
    app_state = AppState::EXIT;
  }
  if (b2World_IsValid(physics_world)) {
    b2DestroyWorld(physics_world);
    physics_world = b2WorldId{};
  }
  for (auto &[name, entry] : texture_atlas) {
    delete entry.texture;
  }
  return app_state;
}
