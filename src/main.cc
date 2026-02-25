#include <glad/glad.h>
// Code block
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <print>

#include "core/quad.h"
#include "core/resource_manager.h"
#include "game.h"
#include "level_editor.h"
#include "menu.h"
#include "state.h"
#include "core/scene.h"
#include "core/input.h"
#include "window.h"
#include "render.h"

void FramebufferSizeCallback(GLFWwindow * /* window */, int width, int height) {
  auto &window = GetGameWindow();
  window.width = width;
  window.height = height;
  window.SetPixelsPerUnit(window.GetPixelsPerUnit());
  window.RecalculateCenteredProjection();
  window.RecalculateScreenSpaceProjection();
  assert(width > 0 && height > 0);
  glViewport(0, 0, width, height);
  render::RecreateFramebuffers(width, height);
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
  auto &window = GetGameWindow();
  window.window = glfwCreateWindow(800, 600, "Dying Anima", nullptr, nullptr);
  if (!window.window) {
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window.window);
  gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
  glfwGetFramebufferSize(window.window, &window.width, &window.height);
  window.SetPixelsPerUnit(window.GetPixelsPerUnit());
  window.RecalculateCenteredProjection();
  window.RecalculateScreenSpaceProjection();
  assert(window.width > 0 && window.height > 0);
  glViewport(0, 0, window.width, window.height);
  glfwSetFramebufferSizeCallback(window.window, FramebufferSizeCallback);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &imgui_io = ImGui::GetIO();
  ResourceManager::Init();
  imgui_io.Fonts->AddFontFromFileTTF(ResourceManager::GetFont("Debug").file.c_str(), 18.5f);
  ImGui_ImplGlfw_InitForOpenGL(window.window, true);
  ImGui_ImplOpenGL3_Init("#version 150");
  render::Init();
  render::UnbindFramebuffer();
  core::quad::Init();
  
  SceneManager scene_manager;
  scene_manager.PushScene(std::make_unique<MenuScene>(scene_manager));
  while (!glfwWindowShouldClose(window.window)) {
    static double last_time = glfwGetTime();
    double current_time = glfwGetTime();
    double delta_time = current_time - last_time;
    glfwPollEvents();
    core::input::Update(window);
    scene_manager.HandleInput();
    scene_manager.Update((float)delta_time);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    scene_manager.Render(window);
    glfwSwapBuffers(window.window);
    last_time = current_time;
    while (window.IsMinimized()) {
      glfwGetFramebufferSize(window.window, &window.width, &window.height);
      glfwPollEvents();
    }
  }
  scene_manager.PopScene();
  core::quad::Quit();
  ResourceManager::Quit();
  render::Quit();
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
  glfwDestroyWindow(window.window);
  glfwTerminate();
  return 0;
}
