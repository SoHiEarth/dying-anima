#ifndef DEGRADATION_H
#define DEGRADATION_H
#include <glm/glm.hpp>
namespace game {
enum DegradationLevel {
  LEVEL_0,  // Softening, artifacts
  LEVEL_1,  // Washed color, log degrades
  LEVEL_2,  // Vignetting, extreme blur. further log degradation
  LEVEL_3   // Silhouettes, dialogue degradation, only combat remains
};
glm::vec3 DegradeColor(const glm::vec3& color, DegradationLevel level);
}

#endif  // DEGRADATION_H