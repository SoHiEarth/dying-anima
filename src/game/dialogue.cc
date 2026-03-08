#include "game/dialogue.h"

#include <pugixml.hpp>

#include "core/camera.h"
#include "core/resource_manager.h"
#include "core/window.h"
#include "util/calculate.h"

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
      meta.dialogue_lines.push_back(line.text().as_string());
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
  static std::shared_ptr<Font> font = nullptr;
  if (!font) {
    font = ResourceManager::GetFont("dialouge").font;
  }

  static std::shared_ptr<Shader> font_shader = nullptr;
  if (!font_shader) {
    font_shader = ResourceManager::GetShader("font").shader;
  }

  static std::shared_ptr<Shader> texture_shader = nullptr;
  if (!texture_shader) {
    texture_shader = ResourceManager::GetShader("texture").shader;
  }

  static std::shared_ptr<Shader> rect_shader = nullptr;
  if (!rect_shader) {
    rect_shader = ResourceManager::GetShader("rect").shader;
  }
  if (dialogue.data.empty()) return;
  if (dialogue.state.current_line >= dialogue.data.size()) {
    std::print(
        "Warn: Dialogue current_line out of bounds, setting to last line\n");
    dialogue.state.current_line = (int)dialogue.data.size() - 1;
  }
  auto& window = GetGameWindow();
  window.SetProjection(ProjectionType::SCREEN_SPACE);
  GetCamera().SetType(CameraType::UI);
  auto meta = dialogue.data.at(dialogue.state.current_line);
  if (meta.character_image) {
    meta.character_image->Render(
        texture_shader,
        CalculateModelMatrix(glm::vec2{50.0f, window.height * 0.25f}, 0.0f,
                             glm::vec2(100.0f, 100.0f)));
  }
  font->RenderUI(meta.character_name, glm::vec2{200.0f, window.height * 0.15f},
                 glm::vec2(1.0f), glm::vec3(1.0f), font_shader);
  for (auto& line : meta.dialogue_lines) {
    font->RenderUI(line, glm::vec2{200.0f, window.height * 0.25f},
                   glm::vec2(1.0f), glm::vec3(1.0f), font_shader);
  }
}