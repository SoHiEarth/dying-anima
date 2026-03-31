#include "core/camera.h"

Camera& GetCamera() {
  static Camera camera;
  return camera;
}
