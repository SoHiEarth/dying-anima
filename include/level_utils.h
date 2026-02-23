#pragma once
#include "object.h"
#include <entt/entt.hpp>
#include <string>
#include <vector>

std::vector<Object> LoadLevel(std::string_view filename, entt::registry &registry);
void SaveLevel(std::string_view filename, const std::vector<Object> &objects);
