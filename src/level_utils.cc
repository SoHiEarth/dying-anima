#include "level_utils.h"

#include "physics.h"
#include "sprite.h"
#include "transform.h"
#include "core/resource_manager.h"
#include <pugixml.hpp>
#include <print>
#include <sstream>
#include "light.h"
#include "render.h"
#include "game/enemy.h"
#include "game/spawn.h"

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
    }
    {
      auto physics_node = object_node.child("Physics");
      if (physics_node) {
        auto& physics = registry.emplace<PhysicsBody>(entity);
        auto& transform = registry.get<Transform>(entity);
        bool is_dynamic = physics_node.attribute("is_dynamic").as_bool();
        bool is_chained = physics_node.attribute("is_chained").as_bool();
        if (physics::WorldValid()) {
          physics.body = physics::CreateBody(transform, is_dynamic);
        }
        physics.is_dynamic = is_dynamic;
      }
    }
    {
      auto light_node = object_node.child("Light");
      if (light_node) {
        auto& light = registry.emplace<Light>(entity);
        light.type =
            static_cast<LightType>(light_node.attribute("type").as_int());
        light.intensity = light_node.attribute("intensity").as_float();
        light.radial_falloff =
            light_node.attribute("radial_falloff").as_float();
        light.volumetric_intensity =
            light_node.attribute("volumetric_intensity").as_float();
        light.color.r = light_node.attribute("color.r").as_float();
        light.color.g = light_node.attribute("color.g").as_float();
        light.color.b = light_node.attribute("color.b").as_float();
      }
    }
    {
      auto enemy_node = object_node.child("PlayerDamager");
      if (enemy_node) {
        auto& enemy = registry.emplace<PlayerDamager>(entity);
        enemy.damage = enemy_node.attribute("damage").as_float();
        enemy.hitbox_radius = enemy_node.attribute("hitbox_radius").as_float();
        enemy.knockback = enemy_node.attribute("knockback").as_float();
        enemy.knockback_direction.x =
            enemy_node.attribute("knockback_direction.x").as_float();
        enemy.knockback_direction.y =
            enemy_node.attribute("knockback_direction.y").as_float();
        enemy.time_until_next_hit =
            enemy_node.attribute("time_until_next_hit").as_float();
      }
    }
    {
      auto spawn_node = object_node.child("PlayerSpawn");
      if (spawn_node) {
        registry.emplace<PlayerSpawn>(entity);
      }
    }
  }
  // Chain physics bodies if marked as chained
  auto view = registry.view<Transform, PhysicsBody>();
  std::vector<glm::vec2> chain_points;
  for (auto entity : view) {
    auto& physics = view.get<PhysicsBody>(entity);
    if (physics.is_chained) {
      auto& transform = view.get<Transform>(entity);
      chain_points.push_back(transform.position);
    }
  }
  if (!chain_points.empty()) {
    physics::CreateChainBody(chain_points);
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
    if (registry.try_get<PhysicsBody>(entity)) {
      auto physics_node = object_node.append_child("Physics");
      physics_node.append_attribute("is_dynamic") = registry.get<PhysicsBody>(entity).is_dynamic;
      physics_node.append_attribute("is_chained") = registry.get<PhysicsBody>(entity).is_chained;
    }
    if (registry.try_get<Light>(entity)) {
      const auto& light = registry.get<Light>(entity);
      auto light_node = object_node.append_child("Light");
      light_node.append_attribute("type") = static_cast<int>(light.type);
      light_node.append_attribute("intensity") = light.intensity;
      light_node.append_attribute("radial_falloff") = light.radial_falloff;
      light_node.append_attribute("volumetric_intensity") =
          light.volumetric_intensity;
      light_node.append_attribute("color.r") = light.color.r;
      light_node.append_attribute("color.g") = light.color.g;
      light_node.append_attribute("color.b") = light.color.b;
    }
    
    // Custom Game components
    if (registry.try_get<PlayerDamager>(entity)) {
      const auto& enemy = registry.get<PlayerDamager>(entity);
      auto enemy_node = object_node.append_child("PlayerDamager");
      enemy_node.append_attribute("damage") = enemy.damage;
      enemy_node.append_attribute("hitbox_radius") = enemy.hitbox_radius;
      enemy_node.append_attribute("knockback") = enemy.knockback;
      enemy_node.append_attribute("knockback_direction.x") =
          enemy.knockback_direction.x;
      enemy_node.append_attribute("knockback_direction.y") =
          enemy.knockback_direction.y;
      enemy_node.append_attribute("time_until_next_hit") =
          enemy.time_until_next_hit;
    }

    if (registry.all_of<PlayerSpawn>(entity)) {
      object_node.append_child("PlayerSpawn");
    }
  }
  int object_count = std::distance(root.children("Object").begin(),
                                   root.children("Object").end());
  printf("Saving level with %d objects\n", object_count);
  if (!doc.save_file(filename.data())) {
    std::print("Failed to save level file: {}\n", filename);
  }
}
