#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "game.h"
#include "player.h"
#include "state.h"
#include "font.h"
#include <memory>
#include "shader.h"
#include <glm/gtc/matrix_transform.hpp>
#include "calculate.h"
#include <format>
#include <print>
#include <box2d/box2d.h>
#include <vector>

// TESTING PURPOSES ONLY
std::vector<glm::vec3> box_positions;

void HandleGameInput(GameState& game_state, Player& player, b2BodyId player_body, GLFWwindow* window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    game_state = GameState::MENU;
  }
  b2Vec2 velocity = b2Body_GetLinearVelocity(player_body);
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    if (velocity.y < player.max_speed)
      velocity.y += player.speed;

  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    if (velocity.y > -player.max_speed)
      velocity.y -= player.speed;

  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    if (velocity.x > -player.max_speed)
      velocity.x -= player.speed;

  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    if (velocity.x < player.max_speed)
      velocity.x += player.speed;

  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_RELEASE && glfwGetKey(window, GLFW_KEY_S) == GLFW_RELEASE)
    velocity.y *= player.deceleration;
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_RELEASE && glfwGetKey(window, GLFW_KEY_D) == GLFW_RELEASE)
    velocity.x *= player.deceleration;

  velocity.x = glm::clamp(velocity.x, -player.max_speed, player.max_speed);
  velocity.y = glm::clamp(velocity.y, -player.max_speed, player.max_speed);

  b2Body_SetLinearVelocity(player_body, velocity);
  
  static bool mouse_pressed_last = false;
  bool mouse_pressed = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
  if (mouse_pressed && !mouse_pressed_last) {
    double mouse_x, mouse_y;
    glfwGetCursorPos(window, &mouse_x, &mouse_y);
    int window_width, window_height;
    glfwGetFramebufferSize(window, &window_width, &window_height);
    float world_x = static_cast<float>(mouse_x);
    float world_y = static_cast<float>(window_height) - static_cast<float>(mouse_y);
    box_positions.push_back(glm::vec3(world_x, world_y, 0.0f));
  }
  mouse_pressed_last = mouse_pressed;
}

GameState Game(GLFWwindow* window) {
  Player player{};
  player.texture = new Texture("assets/textures/player.png");
  auto world_def = b2DefaultWorldDef();
  world_def.gravity = b2Vec2(0.0f, -9.81f);
  auto physics_world = b2CreateWorld(&world_def);
  b2BodyDef body_def = b2DefaultBodyDef();
  body_def.type = b2_dynamicBody;
  body_def.position = b2Vec2(player.position.x, player.position.y);
  b2BodyId player_body = b2CreateBody(physics_world, &body_def);
  b2Polygon shape =
      b2MakeBox(50.0f, 50.0f);
  b2ShapeDef shape_def = b2DefaultShapeDef();
  shape_def.density = 1.0F;
  shape_def.material.friction = 0.3F;
  b2CreatePolygonShape(player_body, &shape_def, &shape);
  
  Shader shader("assets/shaders/simple.vert.glsl", "assets/shaders/simple.frag.glsl");
  shader.Use();
  shader.SetUniform("texture1", 0);

  Shader text_shader("assets/shaders/text.vert.glsl", "assets/shaders/text.frag.glsl");
  text_shader.Use();
  text_shader.SetUniform("character", 0);
  Font font("assets/fonts/Tinos/Tinos-Regular.ttf", 48);

  unsigned int vertex_attrib, vertex_buffer, index_buffer;
  float vertices[] = {
     1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
     1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
    -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
    -1.0f,  1.0f, 0.0f, 0.0f, 1.0f
  };
  unsigned int indices[] = {
    0, 1, 3,
    1, 2, 3
  };

  glGenVertexArrays(1, &vertex_attrib);
  glGenBuffers(1, &vertex_buffer);
  glGenBuffers(1, &index_buffer);
  glBindVertexArray(vertex_attrib);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  GameState game_state = GameState::PLAYING;
  while (!glfwWindowShouldClose(window) && game_state == GameState::PLAYING) {
    glfwPollEvents();
    HandleGameInput(game_state, player, player_body, window);
    b2World_Step(physics_world, 1.0f / 60.0f, 4);
    b2Vec2 position = b2Body_GetPosition(player_body);
    player.position.x = position.x;
    player.position.y = position.y;

    static std::vector<b2BodyId> box_bodies;
    while (box_bodies.size() < box_positions.size()) {
      glm::vec3 box_pos = box_positions[box_bodies.size()];
      b2BodyDef box_body_def = b2DefaultBodyDef();
      box_body_def.type = b2_dynamicBody;
      box_body_def.position = b2Vec2(box_pos.x, box_pos.y);
      b2BodyId box_body = b2CreateBody(physics_world, &box_body_def);
      b2Polygon box_shape = b2MakeBox(25.0f, 25.0f);
      b2ShapeDef box_shape_def = b2DefaultShapeDef();
      box_shape_def.density = 1.0F;
      box_shape_def.material.friction = 0.5F;
      b2CreatePolygonShape(box_body, &box_shape_def, &box_shape);
      box_bodies.push_back(box_body);
    }

    int window_width, window_height;
    glfwGetFramebufferSize(window, &window_width, &window_height);
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(window_width), 0.0f, static_cast<float>(window_height));
    glClear(GL_COLOR_BUFFER_BIT);
    player.texture->Render(
      shader,
      projection,
      glm::mat4(1.0f),
      CalculateModelMatrix(
        player.position,
        glm::vec3(0.0f),
        glm::vec3(50.0f, 50.0f, 1.0f)
      )
    );
    glBindVertexArray(vertex_attrib);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // render a player texture for each box <-- TEMP!
    for (size_t i = 0; i < box_bodies.size(); ++i) {
      b2BodyId box_body = box_bodies[i];
      b2Vec2 box_position = b2Body_GetPosition(box_body);
      player.texture->Render(
        shader,
        projection,
        glm::mat4(1.0f),
        CalculateModelMatrix(
          glm::vec3(box_position.x, box_position.y, 0.0f),
          glm::vec3(0.0f),
          glm::vec3(50.0f, 50.0f, 1.0f)
        )
      );
      glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }
    
    font.Render(std::format("Health: {}", static_cast<int>(player.health)), glm::vec2(20.0f, 20.0f), 1.0f, glm::vec3(1.0f), text_shader, projection);

    // debug info: fps
    static double previous_seconds = glfwGetTime();
    static int frame_count = 0;
    double current_seconds = glfwGetTime();
    frame_count++;
    if (current_seconds - previous_seconds >= 1.0) {
      previous_seconds = current_seconds;
      frame_count = 0;
    }
    double fps = static_cast<double>(frame_count) / (current_seconds - previous_seconds);
    font.Render(std::format("FPS: {:.2f}", fps), glm::vec2(20.0f, 60.0f), 1.0f, glm::vec3(1.0f), text_shader, projection);
    glfwSwapBuffers(window);
  }
  if (game_state == GameState::PLAYING) {
    game_state = GameState::EXIT;
  }
  glDeleteVertexArrays(1, &vertex_attrib);
  glDeleteBuffers(1, &vertex_buffer);
  glDeleteBuffers(1, &index_buffer);
  if (b2World_IsValid(physics_world)) {
    b2DestroyWorld(physics_world);
    physics_world = b2WorldId{};
  }
  delete player.texture;
  return game_state;
}