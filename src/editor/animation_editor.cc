#include <imgui.h>
#include <imgui_stdlib.h>
#include <tinyfiledialogs.h>

#include <entt/entt.hpp>
#include <utility>

#include "editor/animation.h"
#include "core/resource_manager.h"

namespace editor::internal {
bool show_animation_window = false;
}  // namespace editor::internal

void editor::ShowAnimationWindow(bool show) {
  editor::internal::show_animation_window = show;
}

void editor::AnimationWindow(Animation& animation) {
  if (!editor::internal::show_animation_window) {
    return;
  }

  ImGui::Begin("Animation Editor", &editor::internal::show_animation_window);

      for (size_t i = 0; i < animation.frames.size(); ++i) {
        if (ImGui::CollapsingHeader(("Frame " + std::to_string(i)).c_str())) {
        ImGui::Text("Texture: %s", animation.frames[i].texture.tag.c_str());
        ImGui::SameLine();
        if (ImGui::Button(("Change##" + std::to_string(i)).c_str())) {
          ImGui::OpenPopup(("TexturePopup##" + std::to_string(i)).c_str());
        }
        static std::string tag;
        if (ImGui::BeginPopup(("TexturePopup##" + std::to_string(i)).c_str())) {
          if (ImGui::IsWindowAppearing()) {
            tag.clear();
          }
          ImGui::InputText("Texture Tag", &tag);
          if (ImGui::Button(("Load Texture##" + std::to_string(i)).c_str())) {
            auto texture = ResourceManager::GetTexture(tag);
            animation.frames[i].texture = texture;
            ImGui::CloseCurrentPopup();
          }
          ImGui::EndPopup();
        }
        ImGui::InputFloat(("Duration##" + std::to_string(i)).c_str(), &animation.frames[i].duration);
        }
      }
  if (ImGui::Button("Add Frame")) {
    animation.frames.emplace_back(ResourceManager::GetTexture("util.notexture"), 0.1F);
  }

  ImGui::SeparatorText("Utilities");

  if (ImGui::Button("Save Animation")) {
    auto* save_path =
        tinyfd_saveFileDialog("Save Animation", "", 0, nullptr, nullptr);
    if (save_path) {
      SaveAnimationToFile(animation, save_path);
    }
  }

  ImGui::SameLine();
  if (ImGui::Button("Load Animation")) {
    auto* open_path =
        tinyfd_openFileDialog("Load Animation", "", 0, nullptr, nullptr, 0);
    if (open_path) {
      animation = LoadAnimationFromFile(open_path);
    }
  }

  ImGui::End();
}
