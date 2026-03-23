#ifndef DEGRADATION_H
#define DEGRADATION_H
#include <glm/glm.hpp>
namespace game {
enum DegradationLevel {
  kLevel0,  // Softening, artifacts
  kLevel1,  // Washed color, log degrades
  kLevel2,  // Vignetting, extreme blur. further log degradation
  kLevel3   // Silhouettes, dialogue degradation, only combat remains
};
glm::vec3 DegradeColor(const glm::vec3& color, DegradationLevel level);
}  // namespace game

#endif  // DEGRADATION_H