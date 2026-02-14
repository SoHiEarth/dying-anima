#pragma once

#include "state.h"
#include <GLFW/glfw3.h>

namespace menu {
    namespace input {
        void Update(AppState& state, GLFWwindow* window);
        extern int focus_x;
    }
}