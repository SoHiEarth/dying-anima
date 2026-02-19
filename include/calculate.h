#pragma once
#include <glm/glm.hpp>
struct Transform;
glm::mat4 CalculateModelMatrix(const Transform &transform);
glm::mat4 CalculateModelMatrix(const glm::vec2 &position, float rotation,
                               const glm::vec2 &scale);
