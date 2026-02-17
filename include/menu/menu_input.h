#pragma once

#include <GLFW/glfw3.h>

#include "state.h"

namespace menu {
namespace input {
void Update(AppState &state, GLFWwindow *window);
extern int focus_index;
} // namespace input
} // namespace menu
