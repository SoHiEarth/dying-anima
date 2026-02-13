#pragma once
#include <string>
#include <vector>
#include "object.h"
#include <box2d/box2d.h>

std::vector<Object> LoadLevel(std::string_view filename, b2WorldId physics_world);
void SaveLevel(std::string_view filename, const std::vector<Object> &objects);