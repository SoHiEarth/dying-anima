#include "game/intro.h"

#include <imgui.h>

#include <algorithm>
#include <utility>

#include "core/camera.h"
#include "core/input.h"
#include "core/rect.h"
#include "core/resource_manager.h"
#include "game/game.h"
#include "game/progression.h"

namespace {
std::string current_text;
int character_index = 0;
int text_index = 0;
bool update_text = false;
bool show_controls = false;
constexpr float kIntroMargin = 50.0F;
}  // namespace

void IntroScene::Init() {
  text_shader = resource_manager::GetShader("Text").shader;
  rect_shader = resource_manager::GetShader("Rect").shader;
  font = resource_manager::GetFont("Special").font;
}

void IntroScene::Quit() {
  game::save_data.completion_markers.emplace_back(
      completion::kIntroCompleteMarker);
}

void IntroScene::Update(double dt) {
  if (show_controls) {
    return;
  }

  static double timer = 0.0F;
  static double time_since_sentence_complete = 0.0F;
  timer += dt;
  if (current_text.size() == intro_text.at(text_index).size()) {
    time_since_sentence_complete += dt;
    if (time_since_sentence_complete >= 1.5F) {
      update_text = true;
      time_since_sentence_complete = 0.0F;
      current_text = "";
      character_index = 0;
    }
  }
  // 0.5 seconds
  if (timer >= 0.075F) {
    character_index++;
    timer = 0.0F;
    character_index = std::min(character_index,
                               static_cast<int>(intro_text[text_index].size()));
    current_text = intro_text[text_index].substr(0, character_index);
  }
  if (core::input::IsKeyPressedThisFrame(GLFW_KEY_SPACE) || update_text) {
    text_index++;
    update_text = false;
    if (std::cmp_greater_equal(text_index, intro_text.size())) {
      show_controls = true;
    }
  }
}

void IntroScene::Render(GameWindow& window) {
  if (!show_controls) {
    window.SetProjection(ProjectionType::kScreenSpace);
    GetCamera().SetType(CameraType::kUi);
    Rect rect;
    rect.position = glm::vec2(window.width / 2.0F, window.height / 2.0F);
    rect.scale = {window.width, window.height};
    rect.color = glm::vec4(0.0F, 0.0F, 0.0F, 0.8F);
    rect.Render(rect_shader);
    font->Render(current_text, {window.width / 2.0F, window.height / 2.0F},
                 glm::vec2(0.75F), glm::vec3(1.0F), text_shader);
    font->Render(
        "Press Space To Continue",
        {window.width / 2.0F, font->GetHeight("Press Space To Continue")},
        glm::vec2(0.5F), glm::vec3(0.5F), text_shader);
  } else {
    ImGui::SetNextWindowPos(ImVec2(kIntroMargin, kIntroMargin),
                            ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(window.width - (kIntroMargin * 2),
                                    window.height - (kIntroMargin * 2)),
                             ImGuiCond_Always);
    ImGui::Begin("Intro", nullptr,
                 ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
                     ImGuiWindowFlags_NoResize);
    ImGui::Text("Controls");
    ImGui::SeparatorText("Movement");
    ImGui::Image(resource_manager::GetTexture("ui.movement").texture->id,
                 ImVec2(ImGui::GetContentRegionAvail().x, 200),
                 IMGUI_TEXTURE_FLIP);
    ImGui::Text("Use WASD to Move.");
    ImGui::SeparatorText("Journal");
    ImGui::Image(resource_manager::GetTexture("ui.journal").texture->id,
                 ImVec2(ImGui::GetContentRegionAvail().x, 200),
                 IMGUI_TEXTURE_FLIP);
    ImGui::Text("Press TAB to open your journal.");
    ImGui::Text(
        "Your journey throughout Mer will be recorded in your journal.");
    ImGui::SeparatorText("Your Character");
    ImGui::Image(resource_manager::GetTexture("ui.character").texture->id,
                 ImVec2(ImGui::GetContentRegionAvail().x, 200),
                 IMGUI_TEXTURE_FLIP);
    ImGui::Text("You are a lost soul moving throughout Mer.");
    ImGui::TextWrapped(
        "Information about your character can be viewed from the Character "
        "window. Press P to open it.");
    if (ImGui::Button("Start Your Journey",
                      ImVec2(ImGui::GetContentRegionAvail().x, 50))) {
      scene_manager_.PopScene();
    }
    ImGui::End();
  }
}