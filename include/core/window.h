#pragma once
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum class ProjectionType { CENTERED, SCREEN_SPACE };

struct GameWindow {
private:
  ProjectionType current_projection = ProjectionType::CENTERED;
  float pixels_per_unit = 100.0f;
  float half_width = 0.0f, half_height = 0.0f;
    // cache projection matrices
  glm::mat4 centered_projection, screen_space_projection;

public:
  GLFWwindow *window = nullptr;
  int width = 0, height = 0;
  float near = 0.1f, far = 100.0f;
  void SetPixelsPerUnit(float ppu) {
    pixels_per_unit = ppu;
    // calculate half width and height for centered projection (avoid doing this
    // every frame)
    half_width = (width / pixels_per_unit) / 2.0f;
    half_height = (height / pixels_per_unit) / 2.0f;
    RecalculateCenteredProjection();
    RecalculateScreenSpaceProjection();
  }
  float GetPixelsPerUnit() const { return pixels_per_unit; }
  void SetProjection(ProjectionType projection) {
    current_projection = projection;
  }
  glm::mat4 GetProjection() {
    switch (current_projection) {
    case ProjectionType::CENTERED:
      return centered_projection;
    case ProjectionType::SCREEN_SPACE:
      return screen_space_projection;
    }
    return glm::mat4(1.0f);
  }
  void RecalculateCenteredProjection() {
    centered_projection = glm::ortho(-half_width, half_width, -half_height, half_height, near, far);
  }
  void RecalculateScreenSpaceProjection() {
    screen_space_projection = glm::ortho(0.0f, (float)width, 0.0f, (float)height);
  }
  bool IsMinimized() { return (width <= 0 && height <= 0); }
};

GameWindow &GetGameWindow();
