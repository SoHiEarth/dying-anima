#include "game/player.h"

#include <core/window.h>
#include <imgui.h>

#include "game/game.h"

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
    if (cd.manifold.normal.y <= -0.707f) {
      return true;
    }
  }
  return false;
}

// Testing new implementation
bool IsOnGround(b2BodyId body) {
  auto velocity = b2Body_GetLinearVelocity(body);
  return velocity.y == 0;
}
*/

bool IsOnGround(b2BodyId body) {
  int capacity = b2Body_GetContactCapacity(body);
  if (capacity <= 0) return false;

  std::vector<b2ContactData> contacts(capacity);
  int count = b2Body_GetContactData(body, contacts.data(), capacity);
  count = std::min(count, capacity);

  for (int i = 0; i < count; ++i) {
    const b2ContactData& cd = contacts[i];

    if (!(cd.manifold.pointCount > 0)) continue;
    b2Vec2 normal = cd.manifold.normal;
    if (b2Shape_GetBody(cd.shapeIdB).index1 == body.index1) {
      normal.y = -normal.y;
    }

    if (normal.y <= -0.707F) {
      return true;
    }
  }
  return false;
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
  ImGui::Text("Death Count: %d", game::save_data.death_count);

  ImGui::End();
}
