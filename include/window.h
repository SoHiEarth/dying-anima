#pragma once
#include <GLFW/glfw3.h>

struct GameWindow {
  GLFWwindow* window = nullptr;
  int width = 0, height = 0;
  static GameWindow GetGameWindow() {
    static GameWindow window;
    return window;
  }
  bool IsMinimized() {
    return (width <= 0 && height <= 0);
  }
};