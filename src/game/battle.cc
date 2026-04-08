#include <glad/glad.h>
// Code block

#include <GLFW/glfw3.h>
#include <imgui.h>

#include <iostream>
#include <vector>

#include "game/battle.h"
#include "game/degradation.h"
#include "game/game.h"
#include "menu.h"
#include "saves.h"

constexpr float kStaminaDrainPenalty = 10.0F;

namespace {

struct TurnData {
  Enemy* target = nullptr;
  std::vector<Skill> used_skills;
};
enum class BattleTurn { kPlayer, kEnemy };
BattleTurn current_turn = BattleTurn::kPlayer;
TurnData temp_turn_data;
std::optional<TurnData> turn_data;
std::vector<std::string> action_log;

void UseSkill(const Skill& skill, float& user_health, float& user_stamina,
              float& target_health, float& target_stamina,
              const std::string& user_name = "") {
  action_log.emplace_back(user_name + " used " + skill.name + "!");
  user_health -= skill.health_used;
  user_stamina -= skill.stamina_used;
  target_health -= skill.damage;
  target_stamina -= skill.stamina_drain;
  // memo: a skill degradation / skill evolve system might be interesting.
}

void UpdateEnemyAI(std::vector<Enemy>& enemies, Health& player_health_) {
  for (auto& enemy : enemies) {
    std::multimap<int, Skill> skill_rank;
    for (auto& skill : enemy.skills) {
      // 20 hp/20 stamina minimum
      if (enemy.health - 20.0F < skill.health_used) {
        continue;  // don't even think about it.
      }
      if (enemy.stamina - 20.0F < skill.stamina_used) {
        continue;
      }

      // Skill ranking algorithm: damage/used resources - simple, but effective
      // (i think)
      skill_rank.insert({skill.damage / std::max(1.0F, (skill.health_used +
                                                        skill.stamina_used)),
                         skill});
    }

    // Run the action
    if (skill_rank.empty()) {
      action_log.emplace_back("Enemy " + enemy.name + " ran away!");
    } else {
      auto skill_used = skill_rank.rbegin()->second;
      UseSkill(skill_used, enemy.health, enemy.stamina, player_health_.health,
               player_health_.stamina, enemy.name);
    }
  }
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
std::vector<int> defeated_enemy_uids;
}  // namespace

void BattleScene::Init() {
  defeated_enemy_uids.clear();
  // load defeated enemies from save data
  auto save_data = save_manager::LoadLatestSave();
  // check if any enemies in this battle have been defeated before, and if so,
  // remove them from the battle
  for (const auto& uid : save_data.defeated_enemy_uids) {
    defeated_enemy_uids.push_back(uid);
  }
  enemies_.erase(std::remove_if(enemies_.begin(), enemies_.end(),
                                [&](const Enemy& e) {
                                  return std::find(defeated_enemy_uids.begin(),
                                                   defeated_enemy_uids.end(),
                                                   e.uid) !=
                                         defeated_enemy_uids.end();
                                }),
                 enemies_.end());
  assert(!enemies_.empty());
}

void BattleScene::Update(double) {
  switch (current_turn) {
    case BattleTurn::kEnemy:
      UpdateEnemyAI(enemies_, player_health_);
      break;
    default:
      break;
  }
  if (enemies_.empty()) {
    scene_manager_.PopScene();
    scene_manager_.PushScene(std::make_unique<GameScene>(scene_manager_));
  }
  if (turn_data.has_value() && current_turn == BattleTurn::kPlayer) {
    if (turn_data->target != nullptr) {
      for (const auto& skill : turn_data->used_skills) {
        UseSkill(skill, player_health_.health, player_health_.stamina,
                 turn_data->target->health, turn_data->target->stamina,
                 "Player");
      }
      if (turn_data->target->health <= 0 && enemies_.empty()) {
        scene_manager_.PopScene();
        scene_manager_.PushScene(std::make_unique<GameScene>(scene_manager_));
      } else if (turn_data->target->health <= 0) {
        action_log.emplace_back("Enemy " + turn_data->target->name +
                                " was defeated!");
        defeated_enemy_uids.push_back(turn_data->target->uid);
        enemies_.erase(std::remove_if(enemies_.begin(), enemies_.end(),
                                      [&](const Enemy& e) {
                                        return &e == turn_data->target;
                                      }),
                       enemies_.end());
      }
      if (player_health_.stamina <= 0) {
        player_health_.stamina -= kStaminaDrainPenalty;
      }
    }
    turn_data.reset();
    current_turn = BattleTurn::kEnemy;
  }
  if (player_health_.health <= 0) {
    scene_manager_.PopScene();
    scene_manager_.PushScene(std::make_unique<MenuScene>(scene_manager_));
  }
}

void BattleScene::Render(GameWindow& window) {
  auto ui_margin = std::min(window.width, window.height) / 15.0F;
  ImGui::SetNextWindowPos(
      ImVec2(ui_margin, (static_cast<float>(window.height) / 2) - ui_margin),
      ImGuiCond_Always);
  ImGui::SetNextWindowSize(
      ImVec2(window.width - (ui_margin * 2),
             (static_cast<float>(window.height) / 2) - (ui_margin * 2)),
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
      ImGui::SeparatorText("Action Log");
      for (int i = action_log.size() - 1; i > 0; i--) {
        ImGui::Text("%s", action_log.at(i).c_str());
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
          if (ImGui::Button(std::string("Execute!##" + skill.name).c_str())) {
            temp_turn_data.used_skills = {skill};
            if (enemies_.size() > 1)
              ImGui::OpenPopup(
                  std::string("SelectEnemyPopup##" + skill.name).c_str());
            else {
              temp_turn_data.target = enemies_.data();
              turn_data.emplace(temp_turn_data);
              temp_turn_data.target = nullptr;
              temp_turn_data.used_skills.clear();
            }
          }
          if (ImGui::BeginPopup(
                  std::string("SelectEnemyPopup##" + skill.name).c_str())) {
            ImGui::Text("Which enemy do you want to attack?");
            int i = 0;
            for (auto& enemy : enemies_) {
              ImGui::PushID((enemy.name + std::to_string(i++)).c_str());
              if (ImGui::Selectable(enemy.name.c_str(),
                                    &enemy == temp_turn_data.target)) {
                temp_turn_data.target = &enemy;
                turn_data.emplace(temp_turn_data);
                temp_turn_data.target = nullptr;
                temp_turn_data.used_skills.clear();
                ImGui::CloseCurrentPopup();
              }
              ImGui::PopID();
            }
            ImGui::EndPopup();
          }
        }
      }
      if (player_skills_.skills.empty()) {
        ImGui::Text("No skills.");
      }
      int i = 0;
      ImGui::SeparatorText("ENEMY");
      for (const auto& enemy : enemies_) {
        ImGui::PushID((enemy.name + std::to_string(i++)).c_str());
        ImGui::SeparatorText(enemy.name.c_str());
        int j = 0;
        for (const auto& skill : enemy.skills) {
          ImGui::PushID((enemy.name + std::to_string(i) + skill.name + std::to_string(j++)).c_str());
          if (ImGui::CollapsingHeader(skill.name.c_str())) {
            ImGui::Text("Damage: %.2f", skill.damage);
            ImGui::Text("Health Drained: %.2f", skill.health_used);
            ImGui::Text("Stamina Drained: %.2f", skill.stamina_used);
          }
          ImGui::PopID();
        }
        ImGui::PopID();
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
  save_data.completion_markers.push_back(
      "Battle." +
      std::to_string(
          std::chrono::system_clock::now().time_since_epoch().count()) +
      "." + ListEnemyNames(enemies_));
  save_data.log.NewLog(
      {.title = {{game::DegradationLevel::kLevel0,
                  "Battle against " + ListEnemyNames(enemies_)}},
       .description = {{game::DegradationLevel::kLevel0, "No description."}},
       .timestamp = std::to_string(
           std::chrono::system_clock::now().time_since_epoch().count()),
       .texture = {}});
  if (player_health_.health <= 0) {
    save_data.death_count++;
    player_health_.health = 100;
  }
  save_data.player_health.health = player_health_.health;
  save_data.player_health.stamina = player_health_.stamina;
  save_data.defeated_enemy_uids.insert(save_data.defeated_enemy_uids.end(),
                                  defeated_enemy_uids.begin(),
                                       defeated_enemy_uids.end());
  if (save_data.acquired_skills.size() != player_skills_.skills.size()) {
    save_data.acquired_skills = player_skills_.skills;
  }
  save_manager::SaveGame(save_data, save_data.log);
}
