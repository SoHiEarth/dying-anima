#ifndef CAMERA_H
#define CAMERA_H
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum class CameraType {
  WORLD, // Return view matrix based on position
  UI,    // Ignore position
};

struct Camera {
private:
  CameraType type = CameraType::WORLD;

public:
  glm::vec2 position = {0.0f, 0.0f};
  void SetType(CameraType new_type) { type = new_type; }
  CameraType GetType() const { return type; }
  glm::mat4 GetView() {
    switch (type) {
    case CameraType::WORLD:
        return glm::translate(glm::mat4(1.0f), -glm::vec3(position, 3.0f));
    }
    return glm::mat4(1.0f);
  }
};

Camera &GetCamera();

#endif // CAMERA_H
