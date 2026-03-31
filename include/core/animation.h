#ifndef CORE_ANIMATION_H
#define CORE_ANIMATION_H

#include <entt/entt.hpp>
#include <string>
#include <vector>

#include "core/atlas.h"

struct AnimationFrame {
  TextureHandle texture;
  float duration;
};

struct Animation {
  // Set at runtime
  entt::entity entity;
  // Used at runtime.
  std::vector<AnimationFrame> frames;

  bool loop = true;
  // Internal Use!
  float time_since_last_frame = 0.0F;
  // Internal Use!
  size_t current_frame_index = 0;
  // The file path of the animation
  std::string file_path;
};

void UpdateAnimations(entt::registry& registry, float dt);
Animation LoadAnimationFromFile(std::string_view filename);
void SaveAnimationToFile(const Animation& animation, std::string_view filename);

#endif  // CORE_ANIMATION_H
