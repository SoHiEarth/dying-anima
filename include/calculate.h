#pragma once
#include <glm/glm.hpp>

glm::mat4 CalculateModelMatrix(const glm::vec2& position, float rotation,
                               const glm::vec2& scale);