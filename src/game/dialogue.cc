#include "game/dialogue.h"

#include <pugixml.hpp>
#include <utility>

#include "core/camera.h"
#include "core/resource_manager.h"
#include "core/window.h"
#include "util/calculate.h"

constexpr float kDialogueMargin = 50.0f;

DialogueData Game::LoadDialogue(std::string_view file) {
  DialogueData dialogue;
  pugi::xml_document doc;
  if (!doc.load_file(file.data())) {
    throw std::runtime_error("Failed to load dialogue file: " +
                             std::string(file));
  }
  for (const auto& character : doc.child("dialogue").children("character")) {
    DialogueMeta meta;
    meta.character_name = character.attribute("name").as_string();
    meta.character_image =
        ResourceManager::GetTexture(character.attribute("texture").as_string())
            .texture;
    for (const auto& line : character.children("line")) {
      meta.dialogue_lines.emplace_back(line.text().as_string());
    }
    dialogue.data.push_back(std::move(meta));
  }
  return dialogue;
}

void Game::SaveDialogue(const DialogueData& dialogue, std::string_view file) {
  pugi::xml_document doc;
  auto root = doc.append_child("dialogue");
  for (const auto& meta : dialogue.data) {
    auto character = root.append_child("character");
    character.append_attribute("name") = meta.character_name.c_str();
    character.append_attribute("texture") = meta.character_image->path.c_str();
    for (const auto& line : meta.dialogue_lines) {
      auto line_node = character.append_child("line");
      line_node.text() = line.c_str();
    }
  }
  if (!doc.save_file(file.data())) {
    throw std::runtime_error("Failed to save dialogue file: " +
                             std::string(file));
  }
}

void Game::RenderDialogue(DialogueData& dialogue) {
  ImGui::SetNextWindowPos(
      ImVec2(GetGameWindow().width / 2.0f, GetGameWindow().height - 150.0f),
                          ImGuiCond_Always, ImVec2(0.5f, 1.0f));
  ImGui::SetNextWindowSize(ImVec2(GetGameWindow().width - kDialogueMargin * 2.0f, 150.0f),
                           ImGuiCond_Always);
  ImGui::Begin("DialougeWindow", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
                   ImGuiWindowFlags_NoResize);
  if (ImGui::BeginChild("CharacterInfo", ImVec2(150.0f, 0.0f), false)) {
    const auto& meta = dialogue.data[0];
    ImGui::Image(meta.character_image->id,
                 ImVec2(100.0f, 100.0f), ImVec2(1, 1), ImVec2(0, 0));
    ImGui::Text("%s", meta.character_name.c_str());
    ImGui::EndChild();
  }
  if (ImGui::BeginChild("DialogueText", ImVec2(0.0f, 0.0f), false)) {
    const auto& meta = dialogue.data[0];
    ImGui::Text("%s", meta.dialogue_lines[dialogue.state.current_line].c_str());
    ImGui::EndChild();
  }
  ImGui::End();
}
