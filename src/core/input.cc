#include "core/input.h"

#include <map>
#include <print>

std::map<int, bool> core::input::states{},
    core::input::last_frame_states{};

bool core::input::IsKeyPressed(int key) {
  return states.at(key);
}

bool core::input::IsKeyPressedThisFrame(int key) {
  if (last_frame_states.find(key) == last_frame_states.end()) {
    return states.at(key);
  }
  return states.at(key) && !last_frame_states.at(key);
}

void core::input::Update(GameWindow& window) {
  last_frame_states = states;
  states.clear();

  for (int key = GLFW_MOUSE_BUTTON_1; key <= GLFW_MOUSE_BUTTON_LAST; key++) {
    states.insert({key, glfwGetMouseButton(window.window, key) == GLFW_PRESS});
  }

  for (int key = GLFW_KEY_SPACE; key <= GLFW_KEY_LAST; key++) {
    states.insert({key, glfwGetKey(window.window, key) == GLFW_PRESS});
  }
}
