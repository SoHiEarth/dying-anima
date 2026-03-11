#include "core/component.h"

std::vector<ComponentEntry>& GetComponentRegistry() {
  static std::vector<ComponentEntry> registry;
  return registry;
}
