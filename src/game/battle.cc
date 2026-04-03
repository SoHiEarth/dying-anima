#include <glad/glad.h>
// Code block

#include <GLFW/glfw3.h>
#include <imgui.h>

#include <iostream>
#include <thread>
#include <vector>

#include "game/battle.h"
#include "game/degradation.h"
#include "game/game.h"
#include "menu.h"
#include "saves.h"

constexpr float kBattleUIMargin = 50.0F;
constexpr float kStaminaDrainPenalty = 10.0F;

namespace {
struct PlayerTurnData {
  Enemy* target = nullptr;
  std::vector<Skill> used_skills;
};
enum class BattleTurn { kPlayer, kEnemy };
BattleTurn current_turn = BattleTurn::kPlayer;
PlayerTurnData temp_turn_data;
std::optional<PlayerTurnData> turn_data;
void UpdateEnemyAI(std::vector<Enemy>&, double) {
  std::this_thread::sleep_for(std::chrono::seconds(1));
  current_turn = BattleTurn::kPlayer;
}
std::string ListEnemyNames(const std::vector<Enemy>& enemies) {
  std::string result;
  for (size_t i = 0; i < enemies.size(); ++i) {
    result += enemies[i].name;
    if (i != enemies.size() - 1) {
      result += ", ";
    }
  }
  return result;
}
}  // namespace

void BattleScene::Init() {
  // find the player
  assert(!enemies_.empty());
}

void BattleScene::Update(double dt) {
  switch (current_turn) {
    case BattleTurn::kEnemy:
      UpdateEnemyAI(enemies_, dt);
      break;
    default:
      break;
  }
  if (turn_data.has_value() && current_turn == BattleTurn::kPlayer) {
    if (turn_data->target != nullptr) {
      for (const auto& skill : turn_data->used_skills) {
        turn_data->target->health -= skill.damage;
        std::cout << "[BATTLE] Applied " << skill.damage << " damage to target "
                  << turn_data->target << std::endl;
        player_health_.health -= skill.health_used;
        player_health_.stamina -= skill.stamina_used;
      }
      if (turn_data->target->health <= 0) {
        scene_manager_.PopScene();
        scene_manager_.PushScene(std::make_unique<GameScene>(scene_manager_));
      }
    }
    if (player_health_.health <= 0) {
      scene_manager_.PopScene();
      scene_manager_.PushScene(std::make_unique<MenuScene>(scene_manager_));
    }
    if (player_health_.stamina <= 0) {
      player_health_.stamina -= kStaminaDrainPenalty;
    }
    turn_data.reset();
    current_turn = BattleTurn::kEnemy;
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
  if (ImGui::Begin("BattleUI", nullptr,
                   ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
                       ImGuiWindowFlags_NoResize)) {
    if (ImGui::BeginChild("Status",
                          ImVec2(ImGui::GetContentRegionAvail().x / 2, 0))) {
      ImGui::Text("Status");
      ImGui::SeparatorText("PLAYER");
      ImGui::Text("Health: %d", static_cast<int>(player_health_.health));
      ImGui::SeparatorText("ENEMY");
      for (const auto& enemy : enemies_) {
        ImGui::Text("%s Health: %d", enemy.name.c_str(),
                    static_cast<int>(enemy.health));
      }
    }
    ImGui::EndChild();
    ImGui::SameLine();
    if (ImGui::BeginChild("SkillList", ImVec2(0, 0))) {
      ImGui::SeparatorText("PLAYER");
      for (const auto& skill : player_skills_.skills) {
        if (ImGui::CollapsingHeader(skill.name.c_str())) {
          ImGui::Text("Damage: %.2f", skill.damage);
          ImGui::Text("Health Drained: %.2f", skill.health_used);
          ImGui::Text("Stamina Drained: %.2f", skill.stamina_used);
          if (ImGui::Button("Execute!")) {
            temp_turn_data.used_skills = {skill};
            ImGui::OpenPopup("SelectEnemyPopup");
          }
          if (ImGui::BeginPopup("SelectEnemyPopup")) {
            ImGui::Text("Which enemy do you want to attack?");
            for (auto& enemy : enemies_) {
              if (ImGui::Selectable(enemy.name.c_str(),
                                    &enemy == temp_turn_data.target)) {
                temp_turn_data.target = &enemy;
              }
            }
            if (ImGui::Button("Execute Skill") &&
                temp_turn_data.target != nullptr) {
              turn_data.emplace(temp_turn_data);
              temp_turn_data.target = nullptr;
              temp_turn_data.used_skills.clear();
              ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
          }
        }
      }
      if (player_skills_.skills.empty()) {
        ImGui::Text("No skills.");
      }
      ImGui::SeparatorText("ENEMY");
      for (const auto& enemy : enemies_) {
        ImGui::SeparatorText(enemy.name.c_str());
        for (const auto& skill : enemy.skills) {
          if (ImGui::CollapsingHeader(skill.name.c_str())) {
            ImGui::Text("Damage: %.2f", skill.damage);
            ImGui::Text("Health Drained: %.2f", skill.health_used);
            ImGui::Text("Stamina Drained: %.2f", skill.stamina_used);
          }
        }
        if (enemy.skills.empty()) {
          ImGui::Text("No skills.");
        }
      }
    }
    ImGui::EndChild();
    ImGui::End();
  }
}

void BattleScene::Quit() {
  auto save_data = save_manager::LoadLatestSave();
  save_data.completion_markers.push_back("Battle." + std::to_string(std::chrono::system_clock::now().time_since_epoch().count()));
  save_data.log.NewLog({
      .title = {{game::DegradationLevel::kLevel0, "Battle against " + ListEnemyNames(enemies_)}},
      .description = {{game::DegradationLevel::kLevel0, "No description."}},
      .timestamp = std::to_string(std::chrono::system_clock::now().time_since_epoch().count()),
      .texture = {}
  });
  if (player_health_.health <= 0) {
    save_data.death_count++;
    player_health_.health = 100;
  }
  save_data.player_health.health = player_health_.health;
  save_data.player_health.stamina = player_health_.stamina;
  if (save_data.acquired_skills.size() != player_skills_.skills.size()) {
    save_data.acquired_skills = player_skills_.skills;
  }
  save_manager::SaveGame(save_data, save_data.log);
}
