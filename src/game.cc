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
#include <vector>

#include "atlas.h"
#include "calculate.h"
#include "core/input.h"
#include "core/quad.h"
#include "font.h"
#include "game.h"
#include "level_utils.h"
#include "object.h"
#include "physics.h"
#include "player.h"
#include "rect.h"
#include "saves.h"
#include "shader.h"
#include "state.h"
#include "window.h"
#include <filesystem>

std::vector<Object> objects;
glm::vec3 camera_position{0.0f, 0.0f, -20.0f};
b2WorldId physics_world;
int frames_since_no_input = 0;

void HandleGameInput(GameState &game_state, Player &player,
                     b2BodyId player_body, GLFWwindow *window) {
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

  if (core::input::IsKeyPressed(GLFW_KEY_SPACE) && player.IsOnGround()) {
    velocity.y = 0;
  }

  if (core::input::IsKeyPressed(GLFW_KEY_A)) {
    if (velocity.x > -player.max_speed && player.IsOnGround())
      velocity.x -= player.speed;
    if (core::input::IsKeyPressedThisFrame(GLFW_KEY_LEFT_SHIFT)) {
      velocity.x = 0;
    }
  }

  if (core::input::IsKeyPressed(GLFW_KEY_D)) {
    if (velocity.x < player.max_speed && player.IsOnGround())
      velocity.x += player.speed;
    if (core::input::IsKeyPressedThisFrame(GLFW_KEY_LEFT_SHIFT)) {
      velocity.x = 0;
    }
  }

  if (!core::input::IsKeyPressed(GLFW_KEY_A) &&
      !core::input::IsKeyPressed(GLFW_KEY_D)) {
    velocity.x *= player.deceleration;
  }

  velocity.x = glm::clamp(velocity.x, -player.max_speed, player.max_speed);

  if (std::abs(velocity.x) < 0.01f && std::abs(velocity.y) < 0.01f) {
    frames_since_no_input++;
    if (frames_since_no_input - 100 > 500) {
      camera_position.x += (-player.position.x - camera_position.x) * 0.002f;
      camera_position.y += (-player.position.y - camera_position.y) * 0.002f;
      camera_position.z += (-20.0f - camera_position.z) * 0.002f;
      camera_position.z = std::min(camera_position.z, -20.0f);
    }
  } else {
    camera_position.z = std::clamp(camera_position.z - 0.1f, -30.0f, -20.0f);
  }
  b2Body_SetLinearVelocity(player.body, velocity);
  if (core::input::IsKeyPressed(GLFW_KEY_SPACE) && player.IsOnGround()) {
    b2Body_ApplyLinearImpulse(player.body, b2Vec2(0.0f, player.jump_impulse),
                              b2Body_GetWorldCenterOfMass(player.body), true);
  }

  if (core::input::IsKeyPressed(GLFW_KEY_A) &&
      core::input::IsKeyPressedThisFrame(GLFW_KEY_LEFT_SHIFT)) {
    b2Body_ApplyLinearImpulse(player.body, b2Vec2(-player.boost_speed, 0.0f),
                              b2Body_GetWorldCenterOfMass(player.body), true);
  }
  if (core::input::IsKeyPressed(GLFW_KEY_D) &&
      core::input::IsKeyPressedThisFrame(GLFW_KEY_LEFT_SHIFT)) {
    b2Body_ApplyLinearImpulse(player.body, b2Vec2(player.boost_speed, 0.0f),
                              b2Body_GetWorldCenterOfMass(player.body), true);
  }
}

AppState Game(GameWindow window) {
  physics_world = physics::CreatePhysicsWorld({0.0f, -9.81f});
  auto shader_atlas = LoadShaderAtlas("assets/shaders/shader.xml");
  auto font_atlas = LoadFontAtlas("assets/fonts/font.xml");
  auto texture_atlas = LoadTextureAtlas("assets/textures/texture.xml");
  Player player{};
  // Get the latest save data and initialize the player with it
  auto folder = std::filesystem::path("saves");
  if (!std::filesystem::exists(folder) || std::filesystem::is_empty(folder)) {
    std::filesystem::create_directory(folder);
  } else {
    std::string largest_save_id = "";
    for (const auto &entry : std::filesystem::directory_iterator(folder)) {
      if (entry.is_regular_file() && entry.path().extension() == ".save") {
        std::string filename = entry.path().filename().string();
        std::string save_id = filename.substr(0, filename.find(".save"));
        largest_save_id = std::max(largest_save_id, save_id);
      }
    }
    auto save_data = SaveManager::LoadGame("saves/" + largest_save_id + ".save");
    player = save_data.player;
  }
  player.texture_name = "game.player";
  player.body = physics::CreatePhysicsBody(physics_world, player.position,
                                           player.scale, player.rotation, true);

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
    core::input::NewFrame();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    float aspect =
        static_cast<float>(window.width) / static_cast<float>(window.height);

    glm::vec3 screen_player_position =
        glm::vec3(-player.position, 0.0f) - camera_position;
    if (std::abs(screen_player_position.x) > 5.0f) {
      camera_position.x =
          -player.position.x - 5.0f * glm::sign(screen_player_position.x);
    }
    if (std::abs(screen_player_position.y) > 5.0f) {
      camera_position.y =
          -player.position.y - 5.0f * glm::sign(screen_player_position.y);
    }

    glfwPollEvents();
    HandleGameInput(game_state, player, player.body, window.window);
    if (game_state == GameState::RUNNING) {
      b2World_Step(physics_world, 1.0f / 60.0f, 4);
    }
    physics::SyncPosition(player.body, player.position);

    auto view = glm::translate(glm::mat4(1.0f), camera_position);
    auto projection =
        glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    texture_atlas.at(player.texture_name)
        .texture->Render(shader, projection, view,
                         CalculateModelMatrix(player.position, player.rotation,
                                              player.scale));
    core::quad::Render(core::quad::QuadType::WITH_TEXCOORDS);

    for (auto &obj : objects) {
      b2Vec2 physics_position = b2Body_GetPosition(obj.body);
      obj.position = glm::vec2(physics_position.x, physics_position.y);
      if (!texture_atlas.contains(obj.texture_name)) {
        obj.texture_name = "util.notexture";
      }
      shader.Use();
      texture_atlas.at(obj.texture_name)
          .texture->Render(
              shader, projection, view,
              CalculateModelMatrix(obj.position, obj.rotation, obj.scale));
      core::quad::Render(core::quad::QuadType::WITH_TEXCOORDS);
    }

    glm::mat4 ui_projection =
        glm::ortho(0.0f, static_cast<float>(window.width), 0.0f,
                   static_cast<float>(window.height));
    special_font.Render(
        std::format("Health: {}", static_cast<int>(player.health)),
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
    ImGui::DragFloat3("Position", glm::value_ptr(player.position));
    auto velocity = b2Body_GetLinearVelocity(player.body);
    ImGui::Text("Velocity: (%.2f, %.2f)", velocity.x, velocity.y);
    ImGui::Text("On Ground: %s", player.IsOnGround() ? "Yes" : "No");
    ImGui::Text("Camera Position: (%.2f, %.2f, %.2f)", camera_position.x,
                camera_position.y, camera_position.z);
    ImGui::Text("Boxes: %zu", objects.size());
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
    if (objects.empty()) {
      objects = LoadLevel("level.txt", physics_world);
    }
  }
  SaveManager::SaveGame(player);
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
