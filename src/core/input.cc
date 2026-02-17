#include "core/input.h"

#include <map>
#include <print>

std::map<int, bool> core::input::key_states{},
    core::input::last_frame_key_states{};

void core::input::UpdateKeyState(GLFWwindow *window, int key) {
  if (key == GLFW_MOUSE_BUTTON_LEFT || key == GLFW_MOUSE_BUTTON_MIDDLE ||
      key == GLFW_MOUSE_BUTTON_RIGHT) {
    key_states.insert({key, glfwGetMouseButton(window, key) == GLFW_PRESS});
  }
  key_states.insert({key, glfwGetKey(window, key) == GLFW_PRESS});
}

bool core::input::IsKeyPressed(int key) { return key_states.at(key); }

bool core::input::IsKeyPressedThisFrame(int key) {
  return key_states.at(key) && !last_frame_key_states.at(key);
}

void core::input::UpdateLastFrameKeyStates() {
  last_frame_key_states = key_states;
}

void core::input::NewFrame() { key_states.clear(); }