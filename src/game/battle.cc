#include <glad/glad.h>
// Code block

#include <GLFW/glfw3.h>
#include <imgui.h>

#include <vector>

#include "game/battle.h"

constexpr float kBattleUIMargin = 50.0F;

namespace {
struct PlayerTurnData {
  Enemy& target;
  std::vector<Skill> used_skills;
};
enum class BattleTurn { kPlayer, kEnemy };
BattleTurn current_turn = BattleTurn::kPlayer;
bool show_player_turn = false;
std::optional<PlayerTurnData> turn_data;
void UpdateEnemyAI(std::vector<Enemy>&, double) {}

}  // namespace

void BattleScene::Init() {}

void BattleScene::Update(double dt) {
  switch (current_turn) {
    case BattleTurn::kEnemy:
      show_player_turn = false;
      UpdateEnemyAI(enemies_, dt);
      break;
    case BattleTurn::kPlayer:
      show_player_turn = true;
      break;
    default:
      break;
  }
}

void BattleScene::Render(GameWindow& window) {
  ImGui::SetNextWindowPos(
      ImVec2(kBattleUIMargin,
             (static_cast<float>(window.height) / 2) - kBattleUIMargin),
      ImGuiCond_Always);
  ImGui::SetNextWindowSize(
      ImVec2(window.width - (kBattleUIMargin * 2),
             (static_cast<float>(window.height) / 2) - (kBattleUIMargin * 2)),
      ImGuiCond_Always);
  ImGui::Begin("BattleUI", nullptr,
               ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
                   ImGuiWindowFlags_NoResize);
  if (ImGui::BeginChild("Status",
                        ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
    ImGui::Text("Status");
    ImGui::EndChild();
  }
  ImGui::SameLine();
  if (ImGui::BeginChild("SkillList", ImVec2(0, 0))) {
    ImGui::Text("Skills");
    ImGui::EndChild();
  }
  ImGui::End();
}

void BattleScene::Quit() {}
