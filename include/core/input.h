#ifndef INPUT_H
#define INPUT_H

#include <GLFW/glfw3.h>

#include <map>

namespace core::input {
extern std::map<int, bool> key_states;
extern std::map<int, bool> last_frame_key_states;
void UpdateKeyState(GLFWwindow* window, int key);
bool IsKeyPressed(int key);
bool IsKeyPressedThisFrame(int key);
void UpdateLastFrameKeyStates();
void NewFrame();
}  // namespace core::input

#endif