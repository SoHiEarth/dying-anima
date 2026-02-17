#pragma once
#include <box2d/box2d.h>

#include "object.h"
#include <entt/entt.hpp>
#include <string>
#include <vector>

std::vector<Object> LoadLevel(std::string_view filename,
                              entt::registry &registry,
                              b2WorldId physics_world);
void SaveLevel(std::string_view filename, const std::vector<Object> &objects);
