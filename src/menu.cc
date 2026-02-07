#include <glad/glad.h>
#include "menu.h"
#include <GLFW/glfw3.h>
#include <print>
#include <fstream>
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <map>
#include "shader.h"
#include "font.h"
#include "texture.h"
#include "calculate.h"

static int menu_focus_x = 0;
void HandleMenuInput(GameState& state, GLFWwindow* window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    state = GameState::EXIT;
  }
  if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) {
    switch (menu_focus_x) {
      case 0:
        state = GameState::PLAYING;
        break;
      case 1:
        state = GameState::EXIT;
        break;
    }
  }
  if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
    menu_focus_x = 0;
  }
  if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
    menu_focus_x = 1;
  }
}

GameState Menu(GLFWwindow* window) {  
  Font font("assets/fonts/Tinos/Tinos-Regular.ttf", 56);
  Shader text_shader("assets/shaders/text.vert.glsl", "assets/shaders/text.frag.glsl");
  text_shader.Use();
  text_shader.SetUniform("character", 0);

  Shader shader("assets/shaders/simple.vert.glsl", "assets/shaders/simple.frag.glsl");
  shader.Use();
  shader.SetUniform("texture1", 0);

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

  Texture banner_texture("assets/textures/banner.png");

  GameState state = GameState::MENU;
  while (!glfwWindowShouldClose(window) && state == GameState::MENU) {
    glfwPollEvents();
    HandleMenuInput(state, window);
    int window_width, window_height;
    glfwGetFramebufferSize(window, &window_width, &window_height);
    while (window_width <= 0 || window_height <= 0) {
      glfwGetFramebufferSize(window, &window_width, &window_height);
      glfwPollEvents();
    }

    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(window_width), 0.0f, static_cast<float>(window_height));

    glClear(GL_COLOR_BUFFER_BIT);
    banner_texture.Render(
      shader,
      projection,
      glm::mat4(1.0f),
      CalculateModelMatrix(
        glm::vec3(window_width / 2.0f, window_height / 2.0f, 0.0f),
        glm::vec3(0.0f),
        glm::vec3(window_width / 2.0f, window_height * 0.75f, 1.0f)
      )
    );

    glBindVertexArray(vertex_attrib);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    const float menu_y = (float)window_height / 4.0f;
    const int padding = 50;

    int text_width = font.GetWidth("Play", 1.0f) + padding + font.GetWidth("Quit", 1.0f);
    int x_pos = (window_width - text_width) / 2;

    font.Render("Play", glm::vec2(x_pos, menu_y), 1.0f, (menu_focus_x == 0) ? glm::vec3(1.0f, 1.0f, 0.0f) : glm::vec3(1.0f),
      text_shader, projection);
    x_pos += font.GetWidth("Play", 1.0f) + padding;
    font.Render("Quit", glm::vec2(x_pos, menu_y), 1.0f, (menu_focus_x == 1) ? glm::vec3(1.0f, 1.0f, 0.0f) : glm::vec3(1.0f),
      text_shader, projection);
    glfwSwapBuffers(window);
  }

  if (state == GameState::MENU) {
    state = GameState::EXIT;
  }

  glDeleteBuffers(1, &vertex_buffer);
  glDeleteBuffers(1, &index_buffer);
  glDeleteVertexArrays(1, &vertex_attrib);
  return state;
}