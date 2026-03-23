#ifndef COLORS_H
#define COLORS_H

#include <imgui.h>

#include <glm/glm.hpp>

namespace color {
constexpr glm::vec3 kWhite = glm::vec3(1.0F, 1.0F, 1.0F);
constexpr glm::vec3 kBlack = glm::vec3(0.0F, 0.0F, 0.0F);
constexpr glm::vec3 kRed = glm::vec3(1.0F, 0.0F, 0.0F);
constexpr glm::vec3 kGreen = glm::vec3(0.0F, 1.0F, 0.0F);
constexpr glm::vec3 kBlue = glm::vec3(0.0F, 0.0F, 1.0F);
constexpr glm::vec3 kYellow = glm::vec3(1.0F, 1.0F, 0.0F);
constexpr glm::vec3 kCyan = glm::vec3(0.0F, 1.0F, 1.0F);
constexpr glm::vec3 kMagenta = glm::vec3(1.0F, 0.0F, 1.0F);
constexpr glm::vec3 kOrange = glm::vec3(1.0F, 0.5F, 0.0F);
constexpr glm::vec3 kPurple = glm::vec3(0.5F, 0.0F, 0.5F);
constexpr glm::vec3 kGray = glm::vec3(0.5F, 0.5F, 0.5F);
constexpr glm::vec3 kLighterGray = glm::vec3(0.9F);
constexpr glm::vec3 kLightGray = glm::vec3(0.75F, 0.75F, 0.75F);
constexpr glm::vec3 kDarkGray = glm::vec3(0.25F, 0.25F, 0.25F);
constexpr glm::vec3 kPink = glm::vec3(1.0F, 0.75F, 0.8F);
constexpr glm::vec3 kNavy = glm::vec3(0.0F, 0.0F, 0.15F);

ImVec4 ToImVec4(const glm::vec3& color, float alpha = 1.0F);

}  // namespace color
#endif  // COLORS_H