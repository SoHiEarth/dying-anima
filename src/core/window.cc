#include "core/window.h"
#include <GLFW/glfw3.h>
#include "core/log.h"
#include "core/render.h"

namespace {
GameWindow window;
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

void WindowSizeCallback(GLFWwindow* /* window */, int width, int height) {
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

GameWindow& GetGameWindow() { return window; }

void GameWindow::SetWindowSizeType(WindowSizeType type) {
  int window_width;
  int window_height;
  int fb_width;
  int fb_height;
  glfwGetWindowSize(window, &window_width, &window_height);
  glfwGetFramebufferSize(window, &fb_width, &fb_height);
  switch (type) {
    case WindowSizeType::kWindowSize:
      glfwSetWindowSizeCallback(window, WindowSizeCallback);
      width = window_width;
      height = window_height;
      WindowSizeCallback(window, window_width, window_height);
      core::Log("Size type set to window size.", "Window");
      break;
    case WindowSizeType::kFramebufferSize:
      glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
      width = fb_width;
      height = fb_height;
      FramebufferSizeCallback(window, fb_width, fb_height);
      core::Log("Size type set to framebuffer size.", "Window");
      break;
  }
  core::Log(std::format("Window Size: {}x{}, Framebuffer size: {}x{}, Held size: {}x{}", window_width, window_height, fb_width, fb_height, width, height), "Window");
  glfwSetWindowSize(window, window_width, window_height);
  size_type_ = type;
}
