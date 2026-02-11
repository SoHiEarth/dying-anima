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
#include "game.h"
#include "font.h"
#include "level_editor.h"

void FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
  assert(width > 0 && height > 0);
  glViewport(0, 0, width, height);
}

int main() {
  AppState game_state = AppState::MENU;
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
  while (width == 0 || height == 0) {
    glfwGetFramebufferSize(window, &width, &height);
    glfwPollEvents();
  }
  assert(width > 0 && height > 0);
  glViewport(0, 0, width, height);
  glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);

  Shader shader("assets/shaders/text.vert.glsl", "assets/shaders/text.frag.glsl");
  shader.Use();
  shader.SetUniform("character", 0);
  Font opening_font("assets/fonts/Tinos/Tinos-BoldItalic.ttf", 96);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  {
    int window_width, window_height;
    glfwGetFramebufferSize(window, &window_width, &window_height);
    while (window_width == 0 || window_height == 0) {
      glfwGetFramebufferSize(window, &window_width, &window_height);
      glfwPollEvents();
    }

    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(window_width), 0.0f, static_cast<float>(window_height), -1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    opening_font.Render("Dying Anima", glm::vec2((window_width - opening_font.GetWidth("Dying Anima", 1.0f)) / 2.0f, (window_height - opening_font.GetHeight("Dying Anima", 1.0f)) / 2.0f), 1.0f, glm::vec3(1.0f), shader, projection);
    glfwSwapBuffers(window);
    glfwPollEvents();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }

  while (game_state != AppState::EXIT) {
    switch (game_state) {
      case AppState::MENU:
        std::print("Entering Menu\n");
        game_state = Menu(window);
        break;
      case AppState::PLAYING:
        std::print("Starting Game\n");
        game_state = Game(window);
        break;
      case AppState::LEVEL_EDITOR:
        std::print("Entering Level Editor\n");
        game_state = LevelEditor(window);
        break;
      case AppState::EXIT:
        std::print("Exiting Game\n");
        break;
    }
  }
  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}