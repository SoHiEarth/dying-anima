#include "util/colors.h"

const ImVec4 color::ToImVec4(const glm::vec3& color, float alpha) {
  return ImVec4(color.r, color.g, color.b, alpha);
}