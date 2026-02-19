#include "game/rpg_tools.h"
#include <pugixml.hpp>

entt::entity Game::LoadDialogueFromFile(std::string_view file,
                                        entt::registry &registry) {
  auto entity = registry.create();
  auto &meta = registry.emplace<DialogueMeta>(entity);
  pugi::xml_document doc;
  pugi::xml_parse_result result = doc.load_file(file.data());
  if (!result) {
    throw std::runtime_error("Failed to load dialogue file: " +
                             std::string(file));
  }
  auto meta_node = doc.child("Metadata");
  if (meta_node) {
    meta.character_name = meta_node.child("CharacterName").text().as_string();
    meta.character_image = meta_node.child("CharacterImage").text().as_string();
  }
  auto dialogue_node = doc.child("Dialogue");
  if (dialogue_node) {
    for (auto line : dialogue_node.children("Line")) {
      meta.dialogue_lines.push_back(line.text().as_string());
    }
  }
  return entity;
}
