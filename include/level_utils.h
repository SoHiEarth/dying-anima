#pragma once
#include <entt/entt.hpp>
#include <string>

entt::registry LoadLevel(std::string_view filename);
void SaveLevel(std::string_view filename, const entt::registry &registry);
