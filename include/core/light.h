#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>

enum class LightType : int {
	DIRECTIONAL = 0,
	POINT = 1
};

struct Light {
	LightType type = LightType::DIRECTIONAL;
  float intensity = 1.0f;
  float radial_falloff = 1.0f;
  float volumetric_intensity = 0.0f;
  glm::vec3 color = glm::vec3(1.0f);
};

#endif  // LIGHT_H