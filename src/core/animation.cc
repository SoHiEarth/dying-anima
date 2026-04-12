#include "core/animation.h"

#include <pugixml.hpp>

#include "core/resource_manager.h"
#include "core/log.h"
#include "sprite.h"

void UpdateAnimations(entt::registry& registry, float dt) {
  auto view = registry.view<Animation>();
  for (auto entity : view) {
    auto& animation = view.get<Animation>(entity);
    animation.time_since_last_frame += dt;
    if (animation.time_since_last_frame >=
        animation.frames.at(animation.current_frame_index).duration) {
      animation.time_since_last_frame = 0.0F;
      if (animation.loop) {
        animation.current_frame_index =
            (animation.current_frame_index + 1) % animation.frames.size();
      } else {
        animation.current_frame_index = animation.current_frame_index + 1;
      }
      animation.current_frame_index =
          std::min(animation.current_frame_index, animation.frames.size() - 1);
      if (!registry.try_get<Sprite>(entity)) {
        registry.emplace<Sprite>(entity);
      }
      auto& sprite = registry.get<Sprite>(entity);
      sprite.texture =
          animation.frames.at(animation.current_frame_index).texture.texture;
    }
  }
}

Animation LoadAnimationFromFile(std::string_view filename) {
  Animation animation;
  animation.file_path = std::string(filename);
  pugi::xml_document doc;
  if (!doc.load_file(animation.file_path.c_str())) {
    throw core::Error(std::format("Failed to load animation file: {}", filename), "Animation");
  }
  auto root = doc.child("Animation");
  for (auto frame : root.children("Frame")) {
    std::string texture_name = frame.attribute("texture").as_string();
    float duration = frame.attribute("duration").as_float();
    animation.frames.push_back(
        {.texture = resource_manager::GetTexture(texture_name),
         .duration = duration});
  }
  return animation;
}

void SaveAnimationToFile(const Animation& animation,
                         std::string_view filename) {
  pugi::xml_document doc;
  auto root = doc.append_child("Animation");
  for (const auto& frame : animation.frames) {
    auto frame_node = root.append_child("Frame");
    frame_node.append_attribute("texture") = frame.texture.tag.c_str();
    frame_node.append_attribute("duration") = frame.duration;
  }
  doc.save_file(std::string(filename).c_str());
}
