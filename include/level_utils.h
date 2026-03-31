#ifndef LEVEL_UTILS_H
#define LEVEL_UTILS_H

#include <entt/entt.hpp>

entt::registry LoadLevel(std::string_view filename);
void SaveLevel(std::string_view filename, const entt::registry& registry);

#endif  // LEVEL_UTILS_H
