#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "game.h"
#include "player.h"
#include "state.h"
#include "font.h"
#include <memory>
#include "shader.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "calculate.h"
#include <format>
#include <print>
#include <box2d/box2d.h>
#include <vector>
#include "rect.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include "object.h"
#include <pugixml.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include "atlas.h"

// TESTING PURPOSES ONLY
static std::vector<Object> box_positions;
glm::vec3 camera_position{0.0f, 0.0f, -20.0f};
b2WorldId physics_world;
int frames_since_no_input = 0;

void LoadLevel(std::string_view filename) {
  std::fstream file(filename.data());
  if (!file.is_open()) {
    std::print("Failed to open level file: {}\n", filename);
    return;
  }
  box_positions.clear();
  std::string line;
  while (std::getline(file, line)) {
    std::istringstream iss(line);
    Object obj;
    if (!(iss >> obj.position.x >> obj.position.y >> obj.position.z)) {
      std::print("Error reading position in level file: {}\n", line);
      continue;
    }
    if (!(iss >> obj.scale.x >> obj.scale.y >> obj.scale.z)) {
      std::print("Error reading scale in level file: {}\n", line);
      continue;
    }
    if (!(iss >> obj.rotation.x >> obj.rotation.y >> obj.rotation.z)) {
      std::print("Error reading rotation in level file: {}\n", line);
      continue;
    }
    if (!(iss >> obj.texture_name)) {
      std::print("Error reading texture name in level file: {}\n", line);
      continue;
    }
    b2BodyDef box_body_def = b2DefaultBodyDef();
    box_body_def.type = b2_staticBody;
    box_body_def.position = b2Vec2(obj.position.x, obj.position.y);
    obj.body = b2CreateBody(physics_world, &box_body_def);
    b2Polygon box_shape = b2MakeBox(obj.scale.x / 2.0f, obj.scale.y / 2.0f);
    b2ShapeDef box_shape_def = b2DefaultShapeDef();
    box_shape_def.density = 1.0F;
    box_shape_def.material.friction = 0.5F;
    b2CreatePolygonShape(obj.body, &box_shape_def, &box_shape);
    box_positions.push_back(obj);
  }
  file.close();
}

bool IsPlayerOnGround(b2BodyId player_body) {
  int capacity = b2Body_GetContactCapacity(player_body);
  if (capacity <= 0)
    return false;
  std::vector<b2ContactData> contacts(capacity);
  int count = b2Body_GetContactData(player_body, contacts.data(), capacity);
  for (int i = 0; i < count; ++i) {
    b2ContactData &cd = contacts[i];
    if (cd.manifold.normal.y <= -0.9f) {
      return true;
    }
  }
  return false;
}

void HandleGameInput(GameState &game_state, Player &player, b2BodyId player_body, GLFWwindow *window) {
  static bool escape_pressed_last = false;
  bool escape_pressed = glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS;
  if (escape_pressed && !escape_pressed_last) {
    if (game_state == GameState::PAUSED)
      game_state = GameState::RUNNING;
    else if (game_state == GameState::RUNNING)
      game_state = GameState::PAUSED;
  }
  escape_pressed_last = escape_pressed;
  if (game_state == GameState::PAUSED)
    return;
  b2Vec2 velocity = b2Body_GetLinearVelocity(player_body);

  static bool space_pressed_last = false;
  bool space_pressed = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
  if (space_pressed && !space_pressed_last && IsPlayerOnGround(player_body)) {
    velocity.y = 0;
  }
  space_pressed_last = space_pressed;

  static bool left_shift_pressed_last = false;
  bool left_shift_pressed = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;

  static bool left_pressed_last = false;
  bool left_pressed = glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS;
  if (left_pressed) {
    if (velocity.x > -player.max_speed)
      velocity.x -= player.speed;
    if (left_shift_pressed && !left_shift_pressed_last) {
      velocity.x = 0;
    }
  }

  static bool right_pressed_last = false;
  bool right_pressed = glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS;
  if (right_pressed) {
    if (velocity.x < player.max_speed)
      velocity.x += player.speed;
    if (left_shift_pressed && !left_shift_pressed_last) {
      velocity.x = 0;
    }
  }

  if (!left_pressed && !right_pressed) {
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
  b2Body_SetLinearVelocity(player_body, velocity);
  if (space_pressed && IsPlayerOnGround(player_body)) {
    b2Body_ApplyLinearImpulse(
        player_body,
        b2Vec2(0.0f, player.jump_impulse),
        b2Body_GetWorldCenterOfMass(player_body),
        true);
  }
  space_pressed_last = space_pressed;

  if (left_pressed && left_shift_pressed) {
    b2Body_ApplyLinearImpulse(
        player_body,
        b2Vec2(-player.boost_speed, 0.0f),
        b2Body_GetWorldCenterOfMass(player_body),
        true);
  }
  left_pressed_last = left_pressed;
  if (right_pressed && left_shift_pressed) {
    b2Body_ApplyLinearImpulse(
        player_body,
        b2Vec2(player.boost_speed, 0.0f),
        b2Body_GetWorldCenterOfMass(player_body),
        true);
  }
  right_pressed_last = right_pressed;
  left_shift_pressed_last = left_shift_pressed;
}

AppState Game(GLFWwindow *window) {
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  auto shader_atlas = LoadShaderAtlas("assets/shaders/shader.xml");
  auto font_atlas = LoadFontAtlas("assets/fonts/font.xml");
  auto texture_atlas = LoadTextureAtlas("assets/textures/texture.xml");
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &imgui_io = ImGui::GetIO();
  imgui_io.Fonts->AddFontFromFileTTF(font_atlas.at("Debug").file.c_str(), 18.5f);
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 150");

  Player player{};
  player.texture = texture_atlas.find("player")->second.texture;
  auto world_def = b2DefaultWorldDef();
  world_def.gravity = b2Vec2(0.0f, -9.81f);
  physics_world = b2CreateWorld(&world_def);
  b2BodyDef body_def = b2DefaultBodyDef();
  body_def.type = b2_dynamicBody;
  body_def.position = b2Vec2(player.position.x, player.position.y);
  b2BodyId player_body = b2CreateBody(physics_world, &body_def);
  b2Polygon shape = b2MakeBox(player.scale.x / 2.0f, player.scale.y / 2.0f);
  b2ShapeDef shape_def = b2DefaultShapeDef();
  shape_def.density = 1.0F;
  shape_def.material.friction = 0.3F;
  b2CreatePolygonShape(player_body, &shape_def, &shape);

  Shader shader(shader_atlas.at("Sprite").vertex_file, shader_atlas.at("Sprite").fragment_file);
  shader.Use();
  shader.SetUniform("texture1", 0);

  Shader rect_shader(shader_atlas.at("Rect").vertex_file, shader_atlas.at("Rect").fragment_file);
  Shader text_shader(shader_atlas.at("Text").vertex_file, shader_atlas.at("Text").fragment_file);
  text_shader.Use();
  text_shader.SetUniform("character", 0);
  Font special_font(font_atlas.at("Special").file, 48);
  Font title_font(font_atlas.at("Title").file, 96);
  Font ui_font(font_atlas.at("UI").file, 32);

  unsigned int vertex_attrib, vertex_buffer, index_buffer;
  const float vertices[] = {
      0.5f, 0.5f, 0.0f, 1.0f, 1.0f,
      0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
      -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
      -0.5f, 0.5f, 0.0f, 0.0f, 1.0f
  };

  const unsigned int indices[] = {
      0, 1, 3,
      1, 2, 3
  };

  unsigned int rect_vertex_attrib, rect_vertex_buffer, rect_index_buffer;
  const float rect_vertices[] = {
      0.5f, 0.5f,
      0.5f, -0.5f,
      -0.5f, -0.5f,
      -0.5f, 0.5f
  };

  glGenVertexArrays(1, &vertex_attrib);
  glGenBuffers(1, &vertex_buffer);
  glGenBuffers(1, &index_buffer);
  glBindVertexArray(vertex_attrib);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  glGenVertexArrays(1, &rect_vertex_attrib);
  glGenBuffers(1, &rect_vertex_buffer);
  glGenBuffers(1, &rect_index_buffer);
  glBindVertexArray(rect_vertex_attrib);
  glBindBuffer(GL_ARRAY_BUFFER, rect_vertex_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(rect_vertices), rect_vertices, GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rect_index_buffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  AppState app_state = AppState::PLAYING;
  GameState game_state = GameState::RUNNING;
  while (!glfwWindowShouldClose(window) && app_state == AppState::PLAYING) {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    glm::ivec2 screen_dimensions{0, 0};
    glfwGetFramebufferSize(window, &screen_dimensions.x, &screen_dimensions.y);
    assert(screen_dimensions.x > 0 && screen_dimensions.y > 0);
    float aspect = static_cast<float>(screen_dimensions.x) / static_cast<float>(screen_dimensions.y);

    glm::vec3 screen_player_position = glm::vec3(-player.position - camera_position);
    if (std::abs(screen_player_position.x) > 5.0f) {
      camera_position.x = -player.position.x - 5.0f * glm::sign(screen_player_position.x);
    }
    if (std::abs(screen_player_position.y) > 5.0f) {
      camera_position.y = -player.position.y - 5.0f * glm::sign(screen_player_position.y);
    }

    glfwPollEvents();
    HandleGameInput(game_state, player, player_body, window);
    b2World_Step(physics_world, 1.0f / 60.0f, 4);
    b2Vec2 position = b2Body_GetPosition(player_body);
    player.position.x = position.x;
    player.position.y = position.y;
    b2Body_SetFixedRotation(player_body, true);

    auto view = glm::translate(glm::mat4(1.0f), camera_position);
    auto projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    player.texture->Render(
        shader,
        projection,
        view,
        CalculateModelMatrix(
            player.position,
            player.rotation,
            player.scale));
    glBindVertexArray(vertex_attrib);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    for (auto &obj : box_positions) {
      b2Vec2 physics_position = b2Body_GetPosition(obj.body);
      obj.position = glm::vec3(physics_position.x, physics_position.y, obj.position.z);
      if (!texture_atlas.contains(obj.texture_name)) {
        obj.texture_name = "notexture";
      }
      shader.Use();
      texture_atlas.at(obj.texture_name).texture->Render(
          shader,
          projection,
          view,
          CalculateModelMatrix(
              obj.position,
              obj.rotation,
              obj.scale));
      glBindVertexArray(vertex_attrib);
      glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }

    glm::mat4 ui_projection = glm::ortho(0.0f, static_cast<float>(screen_dimensions.x), 0.0f, static_cast<float>(screen_dimensions.y));
    special_font.Render(std::format("Health: {}", static_cast<int>(player.health)), glm::vec2(20.0f, 20.0f), 1.0f, glm::vec3(1.0f), text_shader, ui_projection);

    // debug info: fps
    static double previous_seconds = glfwGetTime();
    static int frame_count = 0;
    double current_seconds = glfwGetTime();
    frame_count++;
    double fps = static_cast<double>(frame_count) / (current_seconds - previous_seconds);
    if (current_seconds - previous_seconds >= 1.0){
      previous_seconds = current_seconds;
      frame_count = 0;
    }

    ImGui::Begin("Debug Menu");
    ImGui::Text("FPS: %.2f", fps);
    ImGui::DragFloat3("Position", glm::value_ptr(player.position));
    auto velocity = b2Body_GetLinearVelocity(player_body);
    ImGui::Text("Velocity: (%.2f, %.2f)", velocity.x, velocity.y);
    ImGui::Text("On Ground: %s", IsPlayerOnGround(player_body) ? "Yes" : "No");
    ImGui::Text("Camera Position: (%.2f, %.2f, %.2f)", camera_position.x, camera_position.y, camera_position.z);
    ImGui::Text("Boxes: %zu", box_positions.size());
    ImGui::Text("Game State: %s", game_state == GameState::RUNNING ? "Running" : "Paused");
    ImGui::End();

    ImGui::Begin("Texture Atlas");
    for (const auto& [name, entry] : texture_atlas) {
        ImGui::Text("Name: %s", name.c_str());
        ImGui::Image(entry.texture->id, ImVec2(100, 100));
    }
    ImGui::End();

    if (game_state == GameState::PAUSED) {
      Rect pause_rect;
      pause_rect.position = glm::vec2(screen_dimensions.x / 2.0f, screen_dimensions.y / 2.0f);
      pause_rect.scale = screen_dimensions;
      pause_rect.color = glm::vec4(0.0f, 0.0f, 0.0f, 0.5f);
      pause_rect.Render(rect_vertex_attrib, rect_shader, ui_projection, glm::mat4(1.0f));

      int x_pos = 30;
      int y_pos = screen_dimensions.y / 2 + 100;
      int padding = 10;
      title_font.Render("PAUSED", glm::vec2(x_pos, y_pos), 1.0f, glm::vec3(1.0f), text_shader, ui_projection);
      y_pos -= title_font.GetHeight("PAUSED", 1.0f) + padding;
      ui_font.Render("Resume", glm::vec2(x_pos, y_pos), 1.0f, glm::vec3(1.0f), text_shader, ui_projection);
      y_pos -= ui_font.GetHeight("Resume", 1.0f) + padding;
      ui_font.Render("Menu", glm::vec2(x_pos, y_pos), 1.0f, glm::vec3(1.0f), text_shader, ui_projection);
      y_pos -= ui_font.GetHeight("Menu", 1.0f) + padding;
      ui_font.Render("Exit", glm::vec2(x_pos, y_pos), 1.0f, glm::vec3(1.0f), text_shader, ui_projection);
#ifndef NDEBUG
      y_pos -= ui_font.GetHeight("Exit", 1.0f) + padding;
      ui_font.Render("Level Editor", glm::vec2(x_pos, y_pos), 1.0f, glm::vec3(1.0f), text_shader, ui_projection);
#endif
    }

    ImGui::Render();
    ImGui::EndFrame();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
    if (box_positions.empty()) {
      LoadLevel("level.txt");
    }
  }
  if (app_state == AppState::PLAYING) {
    app_state = AppState::EXIT;
  }
  glDeleteVertexArrays(1, &vertex_attrib);
  glDeleteBuffers(1, &vertex_buffer);
  glDeleteBuffers(1, &index_buffer);
  if (b2World_IsValid(physics_world)) {
    b2DestroyWorld(physics_world);
    physics_world = b2WorldId{};
  }
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
  for (auto& [name, entry] : texture_atlas) {
    delete entry.texture;
  }
  return app_state;
}