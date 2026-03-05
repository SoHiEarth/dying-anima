#ifndef RPG_TOOLS_H
#define RPG_TOOLS_H

#include <entt/entt.hpp>
#include <string>
#include <vector>
#include "texture.h"
#include "font.h"
#include "shader.h"

struct DialogueMeta {
  std::string character_name;
  std::shared_ptr<Texture> character_image;
  std::vector<std::string> dialogue_lines;
};

struct DialogueState {
  int current_line = 0;
};

struct DialogueData {
  std::vector<DialogueMeta> data{};
  DialogueState state{};
};

namespace Game {
DialogueData LoadDialogue(std::string_view file);
void SaveDialogue(const DialogueData& dialogue, std::string_view file);
void RenderDialogue(DialogueData& dialogue);
} // namespace Game
#endif