#ifndef COMPONENT_H
#define COMPONENT_H

#include <entt/entt.hpp>
#include <functional>
#include <string>
#include <vector>

#include "core/log.h"

struct ComponentEntry {
  std::string name;
  std::function<bool(const entt::registry& registry, entt::entity entity)> Has;
  std::function<void(entt::registry& registry, entt::entity entity)> Add;
};

std::vector<ComponentEntry>& GetComponentRegistry();

template <typename T>
void RegisterComponent(std::string_view name) {
  GetComponentRegistry().push_back(
      {std::string(name),
       [](const entt::registry& registry, entt::entity entity) {
         return registry.any_of<T>(entity);
       },
       [](entt::registry& registry, entt::entity entity) {
         if (!registry.any_of<T>(entity)) {
           registry.emplace<T>(entity);
         }
       }});
  core::Log("Registered component: " + std::string(name), "Component");
}

#define REGISTER_COMPONENT(Component) RegisterComponent<Component>(#Component)

#endif  // COMPONENT_H
