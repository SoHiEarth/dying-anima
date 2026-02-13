#pragma once
#include <map>
#include <GLFW/glfw3.h>

namespace input {
  std::map<int, bool> key_states;
  std::map<int, bool> last_frame_key_states;
  void UpdateKeyState(GLFWwindow* window, int key) {
    key_states[key] = (glfwGetKey(window, key) == GLFW_PRESS);
  }
  bool IsKeyPressed(int key) {
    return key_states[key];
  }
  bool IsKeyPressedThisFrame(int key) {
    return key_states[key] && !last_frame_key_states[key];
  }
  void UpdateLastFrameKeyStates() {
    last_frame_key_states = key_states;
  }
  void NewFrame() {
    key_states.clear();
  }
}