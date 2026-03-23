#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>

enum class LightType : int { kDirectional = 0, kPoint = 1 };

struct Light {
  LightType type = LightType::kDirectional;
  float intensity = 1.0F;
  float radial_falloff = 1.0F;
  float volumetric_intensity = 0.0F;
  glm::vec3 color = glm::vec3(1.0F);
};

#endif  // LIGHT_H