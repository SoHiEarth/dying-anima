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
#include "window.h"
#include "level_editor.h"
#include "core/quad.h"
#include "atlas.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

void FramebufferSizeCallback(GLFWwindow* /* window */, int width, int height) {
  auto window = GameWindow::GetGameWindow();
  window.width = width;
  window.height = height;
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
  auto window = GameWindow::GetGameWindow();
  window.window = glfwCreateWindow(800, 600, "Dying Anima", nullptr, nullptr);
  if (!window.window) {
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window.window);
  gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
  glfwGetFramebufferSize(window.window, &window.width, &window.height);
  while (window.IsMinimized()) {
    glfwGetFramebufferSize(window.window, &window.width, &window.height);
    glfwPollEvents();
  }
  glViewport(0, 0, window.width, window.height);
  glfwSetFramebufferSizeCallback(window.window, FramebufferSizeCallback);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& imgui_io = ImGui::GetIO();
  auto font_atlas = LoadFontAtlas("assets/fonts/font.xml");
  imgui_io.Fonts->AddFontFromFileTTF(font_atlas.at("Debug").file.c_str(), 18.5f);
  ImGui_ImplGlfw_InitForOpenGL(window.window, true);
  ImGui_ImplOpenGL3_Init("#version 150");
  core::quad::Init();
  while (game_state != AppState::EXIT) {
    std::print("Current State: {}\n", static_cast<int>(game_state));
    switch (game_state) {
      case AppState::MENU:
        game_state = Menu(window);
        break;
      case AppState::PLAYING:
        game_state = Game(window);
        break;
      case AppState::LEVEL_EDITOR:
        game_state = LevelEditor(window);
        break;
      case AppState::EXIT:
        break;
    }
  }
  core::quad::Quit();
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
  glfwDestroyWindow(window.window);
  glfwTerminate();
  return 0;
}