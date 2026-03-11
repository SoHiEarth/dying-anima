#include "core/animation.h"

#include <algorithm>
#include <pugixml.hpp>

#include "core/resource_manager.h"
#include "sprite.h"

void PreloadAnimations(Animation& animation, const std::vector<std::string>& names) {
  for (const auto& name : names) {
    LoadAnimation(animation, name);
  }
}

void LoadAnimation(Animation& animation,
                   std::string_view name) {
  for (const auto& [anim_name, frames] : animation.animations) {
    if (anim_name == name) {
      animation.current_animation = anim_name;
      animation.current_frame_index = 0;
      animation.time_since_last_frame = 0.0F;
      animation.frames = std::make_unique<std::vector<AnimationFrame>>(frames);
    }
  }
}

void UpdateAnimations(entt::registry& registry, float dt) {
  auto view = registry.view<Animation>();
  for (auto entity : view) {
    auto& animation = view.get<Animation>(entity);
    if (animation.frames->empty()) continue;
    animation.time_since_last_frame += dt;
    if (animation.time_since_last_frame >=
        animation.frames->at(animation.current_frame_index).duration) {
      animation.time_since_last_frame = 0.0F;
      animation.current_frame_index =
          (animation.current_frame_index + 1) % animation.frames->size();
      if (registry.try_get<Sprite>(entity)) {
        auto& sprite = registry.get<Sprite>(entity);
        sprite.texture = animation.frames->at(animation.current_frame_index).texture.texture;
      } else {
        auto& sprite = registry.emplace<Sprite>(entity);
        sprite.texture = animation.frames->at(animation.current_frame_index).texture.texture;
      }
    }
  }
}

Animation LoadAnimationFromFile(std::string_view filename) {
  Animation animation;
  pugi::xml_document doc;
  if (!doc.load_file(filename.data())) {
    std::print("Failed to load animation file: {}\n", filename);
    return animation;
  }
  auto root = doc.child("Animation");
  animation.current_animation = root.attribute("name").as_string();
  for (auto frame : root.children("Frame")) {
    std::string texture_name = frame.attribute("texture").as_string();
    float duration = frame.attribute("duration").as_float();
    auto texture = ResourceManager::GetTexture(texture_name);
    animation.frames->push_back({texture, duration});
  }
  return animation;
}

void SaveAnimationToFile(const Animation& animation,
                         std::string_view filename) {
  pugi::xml_document doc;
  auto root = doc.append_child("Animation");
  root.append_attribute("name") = animation.current_animation.c_str();
  for (const auto& [name, frames] : animation.animations) {
    for (const auto& frame : frames) {
      auto frame_node = root.append_child("Frame");
      frame_node.append_attribute("texture") = frame.texture.tag.c_str();
      frame_node.append_attribute("duration") = frame.duration;
    }
  }
  doc.save_file(filename.data());
}
