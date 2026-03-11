#ifndef CORE_ANIMATION_H
#define CORE_ANIMATION_H

#include <entt/entt.hpp>
#include <string>
#include <vector>

#include "core/resource_manager.h"

struct AnimationFrame {
  TextureHandle texture;
  float duration;
};

struct Animation {
  // Set at runtime
  entt::entity entity;
  // Used at runtime.
  std::string current_animation;
  // Used at runtime.
  std::unique_ptr<std::vector<AnimationFrame>> frames;
  // Internal Use!
  float time_since_last_frame = 0.0F;
  // Internal Use!
  size_t current_frame_index = 0;
  // Preloaded animations, indexed by name.
  std::vector<std::pair<std::string, std::vector<AnimationFrame>>> animations;
};

void PreloadAnimations(Animation& animation,
                       const std::vector<std::string>& names);
void LoadAnimation(Animation& animation, std::string_view name);
void UpdateAnimations(entt::registry& registry, float dt);
Animation LoadAnimationFromFile(std::string_view filename);
void SaveAnimationToFile(const Animation& animation, std::string_view filename);

#endif  // CORE_ANIMATION_H
