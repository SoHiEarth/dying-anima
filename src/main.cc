#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <fstream>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <print>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <thread>
#include <chrono>
#include "state.h"
#include "menu.h"
#include "texture.h"
#include "shader.h"

void FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
  assert(width > 0 && height > 0);
  glViewport(0, 0, width, height);
}

int main() {
  GameState game_state = GameState::MENU;
  if (!glfwInit()) {
    std::print("Failed to initialize GLFW\n");
    return -1;
  }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  GLFWwindow* window = glfwCreateWindow(800, 600, "Dying Anima", nullptr, nullptr);
  if (!window) {
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
  gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
  int width, height;
  glfwGetFramebufferSize(window, &width, &height);
  assert(width > 0 && height > 0);
  glViewport(0, 0, width, height);
  glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);

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
  
  {
    int window_width, window_height;
    glfwGetFramebufferSize(window, &window_width, &window_height);
    while (window_width == 0 || window_height == 0) {
      glfwGetFramebufferSize(window, &window_width, &window_height);
      glfwPollEvents();
    }

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)window_width / (float)window_height, 0.1f, 100.0f);
    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -1.0f));
    glm::mat4 model = glm::mat4(1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    banner_texture.Render(shader, projection, view, model);
    glBindVertexArray(vertex_attrib);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glfwSwapBuffers(window);
    glfwPollEvents();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }

  while (game_state != GameState::EXIT) {
    switch (game_state) {
      case GameState::MENU:
        game_state = Menu(game_state, window);
        break;
      case GameState::PLAYING:
        // Render game
        break;
      case GameState::PAUSED:
        // Render paused screen
        break;
      case GameState::EXIT:
        break;
    }
  }

  glDeleteBuffers(1, &vertex_buffer);
  glDeleteBuffers(1, &index_buffer);
  glDeleteVertexArrays(1, &vertex_attrib);
  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}