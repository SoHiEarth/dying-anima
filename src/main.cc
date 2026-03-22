#include <glad/glad.h>
// Code block
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <stb_image.h>

#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <print>

#include "core/input.h"
#include "core/quad.h"
#include "core/render.h"
#include "core/resource_manager.h"
#include "core/scene.h"
#include "core/window.h"
#include "menu.h"
#include "util/colors.h"

namespace {
void FramebufferSizeCallback(GLFWwindow* /* window */, int width, int height) {
  auto& window = GetGameWindow();
  window.width = width;
  window.height = height;
  window.SetPixelsPerUnit(window.GetPixelsPerUnit());
  window.RecalculateCenteredProjection();
  window.RecalculateScreenSpaceProjection();
  glViewport(0, 0, width, height);
  render::RecreateFramebuffers(width, height);
}
}  // namespace

int main() {
  if (!glfwInit()) {
    std::print("Failed to initialize GLFW\n");
    return -1;
  }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  auto& window = GetGameWindow();
  window.window = glfwCreateWindow(800, 600, "Dying Anima", nullptr, nullptr);
  if (!window.window) {
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window.window);
  gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));
  glfwGetFramebufferSize(window.window, &window.width, &window.height);
  window.SetPixelsPerUnit(window.GetPixelsPerUnit());
  window.RecalculateCenteredProjection();
  window.RecalculateScreenSpaceProjection();
  glViewport(0, 0, window.width, window.height);
  glfwSetFramebufferSizeCallback(window.window, FramebufferSizeCallback);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& imgui_io = ImGui::GetIO();
  imgui_io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

  ImGuiStyle& style = ImGui::GetStyle();
  style.WindowRounding = 8.0f;
  style.FrameRounding = 2.5f;
  style.ScrollbarRounding = 0;
  style.Colors[ImGuiCol_Text] = ImVec4(0.90f, 0.90f, 0.90f, 0.90f);
  style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
  style.Colors[ImGuiCol_WindowBg] = ImVec4(0.09f, 0.09f, 0.15f, 1.00f);
  style.Colors[ImGuiCol_PopupBg] = ImVec4(0.05f, 0.05f, 0.10f, 0.85f);
  style.Colors[ImGuiCol_Button] = ImVec4(0.48f, 0.72f, 0.89f, 0.49f);
  style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.50f, 0.69f, 0.99f, 0.68f);
  style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.80f, 0.50f, 0.50f, 1.00f);
  style.Colors[ImGuiCol_Header] = ImVec4(0.30f, 0.69f, 1.00f, 0.53f);
  style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.44f, 0.61f, 0.86f, 1.00f);
  style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.38f, 0.62f, 0.83f, 1.00f);

  ResourceManager::Init();
  imgui_io.Fonts->AddFontFromFileTTF(
      ResourceManager::GetFont("Debug").file.c_str(), 15.5F);
  ImGui_ImplGlfw_InitForOpenGL(window.window, true);
  ImGui_ImplOpenGL3_Init("#version 150");
  render::Init();
  core::quad::Init();
  SceneManager scene_manager;
  scene_manager.PushScene(std::make_unique<MenuScene>(scene_manager));
  scene_manager.ProcessSceneChanges();

  double last_time = glfwGetTime();
  constexpr double kMaxDeltaTime = 0.1;

  while (!glfwWindowShouldClose(window.window)) {
    double current_time = glfwGetTime();
    double delta_time = current_time - last_time;
    last_time = current_time;

    delta_time = std::min(delta_time, kMaxDeltaTime);

    glfwPollEvents();
    core::input::Update(window);
#ifndef NDEBUG
    if (core::input::IsKeyPressedThisFrame(GLFW_KEY_F2)) {
      // wireframe or filled
      static bool wireframe = false;
      wireframe = !wireframe;
      if (wireframe) {
        glLineWidth(2.0F);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      }
    }
#endif
    if (core::input::IsKeyPressedThisFrame(GLFW_KEY_F11)) {
      static bool fullscreen = false;
      fullscreen = !fullscreen;
      if (fullscreen) {
        const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        glfwSetWindowMonitor(window.window, glfwGetPrimaryMonitor(), 0, 0,
                             mode->width, mode->height, mode->refreshRate);
      } else {
        glfwSetWindowMonitor(window.window, nullptr, 100, 100, 800, 600, 0);
      }
    }
    scene_manager.Update(delta_time);
    glClearColor(0.0F, 0.0F, 0.0F, 1.0F);
    glClear(GL_COLOR_BUFFER_BIT);
    scene_manager.Render(window);
    glfwSwapBuffers(window.window);

    while (window.IsMinimized()) {
      glfwGetFramebufferSize(window.window, &window.width, &window.height);
      glfwPollEvents();
      last_time = glfwGetTime();
    }
    if (scene_manager.NoScenes()) {
      glfwSetWindowShouldClose(window.window, 1);
    }
    scene_manager.ProcessSceneChanges();
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
