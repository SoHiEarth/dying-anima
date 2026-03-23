#include "game/degradation.h"

glm::vec3 game::DegradeColor(const glm::vec3& color, DegradationLevel level) {
  switch (level) {
    case kLevel0:
      return color;
    case kLevel1:
      return color * 0.75F;
    case kLevel2:
      return color * 0.5F;
    case kLevel3:
      return color * 0.25F;
    default:
      return color;
  }
}