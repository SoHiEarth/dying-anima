#include "level_utils.h"

#include <fstream>
#include <print>
#include <sstream>

std::vector<Object> LoadLevel(std::string_view filename,
                              b2WorldId physics_world) {
  std::vector<Object> objects{};
  std::fstream file(filename.data());
  if (!file.is_open()) {
    std::print("Failed to open level file: {}\n", filename);
    return {};
  }
  std::string line;
  while (std::getline(file, line)) {
    std::istringstream iss(line);
    Object obj;
    if (!(iss >> obj.position.x >> obj.position.y)) {
      std::print("Error reading position in level file: {}\n", line);
      continue;
    }
    if (!(iss >> obj.scale.x >> obj.scale.y)) {
      std::print("Error reading scale in level file: {}\n", line);
      continue;
    }
    if (!(iss >> obj.rotation)) {
      std::print("Error reading rotation in level file: {}\n", line);
      continue;
    }
    if (!(iss >> obj.texture_name)) {
      std::print("Error reading texture name in level file: {}\n", line);
      continue;
    }
    b2BodyDef box_body_def = b2DefaultBodyDef();
    box_body_def.type = b2_staticBody;
    box_body_def.position = b2Vec2(obj.position.x, obj.position.y);
    obj.body = b2CreateBody(physics_world, &box_body_def);
    b2Polygon box_shape = b2MakeBox(obj.scale.x / 2.0f, obj.scale.y / 2.0f);
    b2ShapeDef box_shape_def = b2DefaultShapeDef();
    box_shape_def.density = 1.0F;
    box_shape_def.material.friction = 0.5F;
    b2CreatePolygonShape(obj.body, &box_shape_def, &box_shape);
    objects.push_back(obj);
  }
  file.close();
  return objects;
}

void SaveLevel(std::string_view filename, const std::vector<Object> &objects) {
  std::ofstream file(filename.data());
  if (!file.is_open()) {
    std::print("Failed to open level file for writing: {}\n", filename);
    return;
  }
  for (const auto &obj : objects) {
    file << obj.position.x << " " << obj.position.y << " ";
    file << obj.scale.x << " " << obj.scale.y << " ";
    file << obj.rotation << " ";
    file << "util.notexture" << "\n";
  }
  file.close();
}