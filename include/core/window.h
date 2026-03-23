#pragma once
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum class ProjectionType { kCentered, kScreenSpace };

struct GameWindow {
 private:
  ProjectionType current_projection_ = ProjectionType::kCentered;
  float pixels_per_unit_ = 100.0F;
  float half_width_ = 0.0F, half_height_ = 0.0F;
  // cache projection matrices
  glm::mat4 centered_projection_, screen_space_projection_;

 public:
  GLFWwindow* window = nullptr;
  int width = 0, height = 0;
  float near = 0.1F, far = 100.0F;
  void SetPixelsPerUnit(float ppu) {
    pixels_per_unit_ = ppu;
    // calculate half width and height for centered projection (avoid doing this
    // every frame)
    half_width_ = (width / pixels_per_unit_) / 2.0F;
    half_height_ = (height / pixels_per_unit_) / 2.0F;
    RecalculateCenteredProjection();
    RecalculateScreenSpaceProjection();
  }
  float GetPixelsPerUnit() const { return pixels_per_unit_; }
  void SetProjection(ProjectionType projection) {
    current_projection_ = projection;
  }
  glm::mat4 GetProjection() {
    switch (current_projection_) {
      case ProjectionType::kCentered:
        return centered_projection_;
      case ProjectionType::kScreenSpace:
        return screen_space_projection_;
    }
    return glm::mat4(1.0F);
  }
  void RecalculateCenteredProjection() {
    centered_projection_ = glm::ortho(-half_width_, half_width_, -half_height_,
                                      half_height_, near, far);
  }
  void RecalculateScreenSpaceProjection() {
    screen_space_projection_ = glm::ortho(0.0F, static_cast<float>(width), 0.0F,
                                          static_cast<float>(height));
  }
  bool IsMinimized() const { return (width <= 0 && height <= 0); }
};

GameWindow& GetGameWindow();
