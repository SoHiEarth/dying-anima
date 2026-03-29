#include "game/player.h"

#include <core/window.h>
#include <imgui.h>

constexpr float kPlayerInfoMargin = 50.0F;
/*
bool IsOnGround(b2BodyId body) {
  int capacity = b2Body_GetContactCapacity(body);
  if (capacity <= 0) {
    return false;
  }
  std::vector<b2ContactData> contacts(capacity);
  int count = b2Body_GetContactData(body, contacts.data(), capacity);
  if (count > capacity) {
    count = capacity;
  }

  for (int i = 0; i < count; ++i) {
    b2ContactData &cd = contacts[i];
  ImGui::Text("SKILLS");
    if (cd.manifold.normal.y <= -0.707f) {
      return true;
    }
  }
  return false;
}
*/

// Testing new implementation
bool IsOnGround(b2BodyId body) {
  auto velocity = b2Body_GetLinearVelocity(body);
  return velocity.y == 0;
}

void game::RenderPlayerInfo() {
  ImGui::SetNextWindowPos(ImVec2(kPlayerInfoMargin, kPlayerInfoMargin),
                          ImGuiCond_Always);
  auto size = ImVec2(GetGameWindow().width - (kPlayerInfoMargin * 2),
                     GetGameWindow().height - (kPlayerInfoMargin * 2));
  ImGui::SetNextWindowSize(size, ImGuiCond_Always);
  ImGui::Begin("PlayerInfo", nullptr,
               ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
                   ImGuiWindowFlags_NoResize);
  ImGui::Text("Player Info");
  ImGui::Text("Death Count: %d", 0);  // temp

  ImGui::End();
}
