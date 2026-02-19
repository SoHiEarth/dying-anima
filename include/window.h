#pragma once
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum class ProjectionType { CENTERED, SCREEN_SPACE };

struct GameWindow {
private:
  ProjectionType current_projection = ProjectionType::CENTERED;
  glm::mat4 GetCenteredProjection() {
    return glm::ortho(-(float)width / 2, (float)width / 2, -(float)height / 2,
                      (float)height / 2, near, far);
  }
  glm::mat4 GetScreenSpaceProjection() {
    return glm::ortho(0.0f, (float)width, 0.0f, (float)height);
  }

public:
  GLFWwindow *window = nullptr;
  int width = 0, height = 0;
  float near = 0.1f, far = 100.0f;
  void SetProjection(ProjectionType projection) {
    current_projection = projection;
  }
  glm::mat4 GetProjection() {
    switch (current_projection) {
    case ProjectionType::CENTERED:
      return GetCenteredProjection();
    case ProjectionType::SCREEN_SPACE:
      return GetScreenSpaceProjection();
    }
  }
  bool IsMinimized() { return (width <= 0 && height <= 0); }
};

GameWindow &GetGameWindow();
