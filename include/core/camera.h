#ifndef CAMERA_H
#define CAMERA_H
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum class CameraType {
  kWorld,  // Return view matrix based on position
  kUi,     // Ignore position
};

struct Camera {
 private:
  CameraType type_ = CameraType::kWorld;

 public:
  glm::vec2 position = {0.0F, 0.0F};
  void SetType(CameraType new_type) { type_ = new_type; }
  CameraType GetType() const { return type_; }
  glm::mat4 GetView() {
    switch (type_) {
      case CameraType::kWorld:
        return glm::translate(glm::mat4(1.0F), -glm::vec3(position, 3.0F));
      case CameraType::kUi:
        break;
    }
    return glm::mat4(1.0F);
  }
};

Camera& GetCamera();

#endif  // CAMERA_H
