#ifndef RPG_TOOLS_H
#define RPG_TOOLS_H

#include <entt/entt.hpp>
#include <string>
#include <vector>

struct DialogueMeta {
  std::string character_name;
  std::string character_image;
  std::vector<std::string> dialogue_lines;
  int current_line = 0;
};

namespace Game {
entt::entity LoadDialogueFromFile(std::string_view file,
                                  entt::registry &registry);

void RenderDialouges(entt::registry &registry);
} // namespace Game
#endif
