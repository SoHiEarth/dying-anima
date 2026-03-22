#ifndef COLORS_H
#define COLORS_H

#include <glm/glm.hpp>
#include <imgui.h>

namespace color {
constexpr glm::vec3 WHITE = glm::vec3(1.0f, 1.0f, 1.0f);
constexpr glm::vec3 BLACK = glm::vec3(0.0f, 0.0f, 0.0f);
constexpr glm::vec3 RED = glm::vec3(1.0f, 0.0f, 0.0f);
constexpr glm::vec3 GREEN = glm::vec3(0.0f, 1.0f, 0.0f);
constexpr glm::vec3 BLUE = glm::vec3(0.0f, 0.0f, 1.0f);
constexpr glm::vec3 YELLOW = glm::vec3(1.0f, 1.0f, 0.0f);
constexpr glm::vec3 CYAN = glm::vec3(0.0f, 1.0f, 1.0f);
constexpr glm::vec3 MAGENTA = glm::vec3(1.0f, 0.0f, 1.0f);
constexpr glm::vec3 ORANGE = glm::vec3(1.0f, 0.5f, 0.0f);
constexpr glm::vec3 PURPLE = glm::vec3(0.5f, 0.0f, 0.5f);
constexpr glm::vec3 GRAY = glm::vec3(0.5f, 0.5f, 0.5f);
constexpr glm::vec3 LIGHTER_GRAY = glm::vec3(0.9f);
constexpr glm::vec3 LIGHT_GRAY = glm::vec3(0.75f, 0.75f, 0.75f);
constexpr glm::vec3 DARK_GRAY = glm::vec3(0.25f, 0.25f, 0.25f);
constexpr glm::vec3 PINK = glm::vec3(1.0f, 0.75f, 0.8f);
constexpr glm::vec3 NAVY = glm::vec3(0.0f, 0.0f, 0.15f);

const ImVec4 ToImVec4(const glm::vec3& color, float alpha = 1.0f);

}  // namespace colors
#endif // COLORS_H