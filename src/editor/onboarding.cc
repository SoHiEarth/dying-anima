#include "editor/onboarding.h"

#include <imgui.h>

#include "core/resource_manager.h"
#include "core/window.h"

constexpr float kMargin = 150.0F;
bool editor::internal::show_onboarding_window = true;

void editor::RenderOnboarding() {
  ImGui::SetNextWindowPos(
      ImVec2(GetGameWindow().width / 2.0F, GetGameWindow().height / 2.0F),
      ImGuiCond_Always, ImVec2(0.5F, 0.5F));
  ImGui::SetNextWindowSize(
      ImVec2(GetGameWindow().width - kMargin, GetGameWindow().height - kMargin),
      ImGuiCond_Always);
  ImGui::Begin("Onboarding", &editor::internal::show_onboarding_window,
               ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
  ImGui::SeparatorText("Hey! I see you're new here.");
  ImGui::Text(
      "Here's some reference material to get you started with the level "
      "editor.");
  if (ImGui::BeginTabBar("Tab")) {
    if (ImGui::BeginTabItem("Layout")) {
      ImGui::SeparatorText(
          "Learn more about the layout of the editor and its features.");
      auto avail_width = ImGui::GetContentRegionAvail().x;

      ImGui::SeparatorText("VIEWPORT");
      auto viewport_texture =
          resource_manager::GetTexture("doc.viewport").texture;
      ImGui::Image(
          viewport_texture->id,
          ImVec2(avail_width, viewport_texture->height *
                                  (avail_width / viewport_texture->width)),
          IMGUI_TEXTURE_FLIP);
      ImGui::TextWrapped(
          "The middle is the viewport. You can view the scene from here. For "
          "convinenince, your zoom settings in the editor is passed down to "
          "the game temporarily, so you can debug scenes with ease.");
      ImGui::SeparatorText("SCENE");
      auto scene_texture = resource_manager::GetTexture("doc.scene").texture;
      ImGui::Image(
          scene_texture->id,
          ImVec2(avail_width, scene_texture->height *
                                  (avail_width / scene_texture->width)),
          IMGUI_TEXTURE_FLIP);
      ImGui::TextWrapped(
          "To the left is the scene manager. You can use it to select entities "
          "and change the editing mode.");
      ImGui::SeparatorText("INSPECTOR");
      auto inspector_texture =
          resource_manager::GetTexture("doc.inspector").texture;
      ImGui::Image(
          inspector_texture->id,
          ImVec2(avail_width, inspector_texture->height *
                                  (avail_width / inspector_texture->width)),
          IMGUI_TEXTURE_FLIP);
      ImGui::TextWrapped(
          "The right is the entity inspector. You can use it to manipulate "
          "various properties on entities, add or remove components and delete "
          "entities altogether.");
      ImGui::SeparatorText("RESOURCE MANAGER");
      auto resource_texture =
          resource_manager::GetTexture("doc.resource").texture;
      ImGui::Image(
          resource_texture->id,
          ImVec2(avail_width, resource_texture->height *
                                  (avail_width / resource_texture->width)),
          IMGUI_TEXTURE_FLIP);
      ImGui::TextWrapped(
          "Below is the Resource Manager. You can drag items from below into "
          "the inspector.");
      ImGui::EndTabItem();
    }
    ImGui::EndTabBar();
  }
  ImGui::Text("Once you're finished, start using the editor:");
  ImGui::SameLine();
  if (ImGui::Button("Start Now!")) {
    editor::internal::show_onboarding_window = false;
  }
  ImGui::End();
}
