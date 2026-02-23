#ifndef INPUT_H
#define INPUT_H

#include <map>
#include "window.h"

namespace core::input {
extern std::map<int, bool> states;
extern std::map<int, bool> last_frame_states;
void Update(GameWindow& window);
bool IsKeyPressed(int key);
bool IsKeyPressedThisFrame(int key);
} // namespace core::input

#endif