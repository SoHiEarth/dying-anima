#include "level_utils.h"

#include "physics.h"
#include "sprite.h"
#include "transform.h"
#include "core/resource_manager.h"
#include <pugixml.hpp>
#include <print>
#include <sstream>

entt::registry LoadLevel(std::string_view filename) {
  pugi::xml_document doc;
  if (!doc.load_file(filename.data())) {
    std::print("Failed to load level file: {}\n", filename);
    return {};
  }
  auto registry = entt::registry{};
  auto root = doc.child("Level");
  int object_count = std::distance(root.children("Object").begin(),
                                   root.children("Object").end());
  printf("Loading level with %d objects\n", object_count);
  for (auto object_node : root.children("Object")) {
    auto entity = registry.create();
    {
      auto transform_node = object_node.child("Transform");
      auto& transform = registry.emplace<Transform>(entity);
      transform.position.x = transform_node.attribute("pos.x").as_float();
      transform.position.y = transform_node.attribute("pos.y").as_float();
      transform.scale.x = transform_node.attribute("scale.x").as_float();
      transform.scale.y = transform_node.attribute("scale.y").as_float();
      transform.rotation = transform_node.attribute("rotation").as_float();
    }
    {
      auto sprite_node = object_node.child("Sprite");
      auto& sprite = registry.emplace<Sprite>(entity);
      sprite.texture_tag = sprite_node.attribute("texture_tag").as_string();
      sprite.texture = ResourceManager::GetTexture(sprite.texture_tag).texture;
      sprite.texture = ResourceManager::GetTexture(sprite.texture_tag).texture;
    }
  }
  return registry;
}

void SaveLevel(std::string_view filename, const entt::registry& registry) {
  pugi::xml_document doc;
  auto view = registry.view<Transform, Sprite>();
  auto root = doc.append_child("Level");
  for (auto entity : view) {
    auto object_node = root.append_child("Object");
    {
      const auto& transform = view.get<Transform>(entity);
      auto transform_node = object_node.append_child("Transform");
      transform_node.append_attribute("pos.x") = transform.position.x;
      transform_node.append_attribute("pos.y") = transform.position.y;
      transform_node.append_attribute("scale.x") = transform.scale.x;
      transform_node.append_attribute("scale.y") = transform.scale.y;
      transform_node.append_attribute("rotation") = transform.rotation;
    }
    {
      const auto& sprite = view.get<Sprite>(entity);
      auto sprite_node = object_node.append_child("Sprite");
      sprite_node.append_attribute("texture_tag") = sprite.texture_tag.c_str();
    }
  }
  int object_count = std::distance(root.children("Object").begin(),
                                   root.children("Object").end());
  printf("Saving level with %d objects\n", object_count);
  if (!doc.save_file(filename.data())) {
    std::print("Failed to save level file: {}\n", filename);
  }
}
