#include "pause.h"

GameState Pause(GLFWwindow* window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    return GameState::PLAYING;
  }
  return GameState::PAUSED;
}