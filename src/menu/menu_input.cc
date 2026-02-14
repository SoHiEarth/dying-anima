#include <algorithm>
#include <GLFW/glfw3.h>
#include "core/input.h"
#include "menu/menu_input.h"
#include "state.h"

int menu::input::focus_x = 0;

void menu::input::Update(AppState& state, GLFWwindow* window) {
  core::input::NewFrame();
  core::input::UpdateKeyState(window, GLFW_KEY_ESCAPE);
  core::input::UpdateKeyState(window, GLFW_KEY_ENTER);
  core::input::UpdateKeyState(window, GLFW_KEY_LEFT);
  core::input::UpdateKeyState(window, GLFW_KEY_RIGHT);

  if (core::input::IsKeyPressed(GLFW_KEY_ESCAPE)) {
    state = AppState::EXIT;
  }
  if (core::input::IsKeyPressed(GLFW_KEY_ENTER)) {
    state = static_cast<AppState>(menu::input::focus_x);
  }
  if (core::input::IsKeyPressedThisFrame(GLFW_KEY_LEFT)) {
    menu::input::focus_x = std::max(menu::input::focus_x - 1, 0);
  }
  if (core::input::IsKeyPressedThisFrame(GLFW_KEY_RIGHT)) {
#ifdef NDEBUG
    const int max_focus = 1;
#else
    const int max_focus = 2;
#endif
    menu::input::focus_x = std::min(menu::input::focus_x + 1, max_focus);
  }
  
  core::input::UpdateLastFrameKeyStates();
}