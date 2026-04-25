#include <glad/glad.h>
// Code block

#include <GLFW/glfw3.h>
#include <imgui.h>

#include <algorithm>
#include <vector>

#include "core/log.h"
#include "core/render.h"
#include "game/battle.h"
#include "core/window.h"
#include "game/degradation.h"
#include "game/game.h"
#include "level_utils.h"
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
std::optional<std::string> ListEnemyNames(const std::vector<Enemy>& enemies) {
  std::string result;
  for (size_t i = 0; i < enemies.size(); ++i) {
    result += enemies[i].name;
    if (i != enemies.size() - 1) {
      result += ", ";
    }
  }
  if (result.empty()) {
    return std::nullopt;
  }
  return result;
}
std::vector<int> defeated_enemies;
}  // namespace

entt::registry battle::registry;

void BattleScene::Init() {
  GetGameWindow().SetWindowSizeType(WindowSizeType::kFramebufferSize);
  defeated_enemies.clear();
  // load defeated enemies from save data
  std::expected<SaveData, LoadError> save_data;
  try {
    save_data = save_manager::LoadLatestSave();
  } catch (std::exception& e) {
    core::Log(std::format("Caught error {} while loading save.", e.what()),
              "BattleScene");
  }

  if (save_data.has_value()) {
    defeated_enemies = save_data->defeated_enemy_uids;
    auto enemies_to_erase =
        std::ranges::remove_if(enemies_, [&](const Enemy& e) {
          return std::ranges::find(defeated_enemies, e.uid) !=
                 defeated_enemies.end();
        });
    enemies_.erase(enemies_to_erase.begin(), enemies_to_erase.end());
  }
  if (enemies_.empty()) {
    core::Log("Logic Error: Enemy count empty.", "BattleScene");
    scene_manager_.PopScene();
  }
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
      } else if (turn_data->target->health <= 0) {
        action_log.emplace_back("Enemy " + turn_data->target->name +
                                " was defeated!");
        defeated_enemies.push_back(turn_data->target->uid);
        auto enemies_to_erase = std::ranges::remove_if(
            enemies_, [&](const Enemy& e) { return &e == turn_data->target; });
        enemies_.erase(enemies_to_erase.begin(), enemies_to_erase.end());
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
  int window_width;
  int window_height;
  glfwGetWindowSize(window.window, &window_width, &window_height);
  auto ui_margin = std::min(window_width, window_height) / 15.0F;
  ImGui::SetNextWindowPos(
      ImVec2(ui_margin, ui_margin),
      ImGuiCond_Always);
  ImGui::SetNextWindowSize(
      ImVec2(window_width - (ui_margin * 2),
             static_cast<float>(window_height) - (ui_margin * 2)),
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
            if (enemies_.size() > 1) {
              ImGui::OpenPopup(
                  std::string("SelectEnemyPopup##" + skill.name).c_str());
            } else {
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
          ImGui::PushID((enemy.name + std::to_string(i) + skill.name +
                         std::to_string(j++))
                            .c_str());
          if (ImGui::CollapsingHeader(skill.name.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
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
  std::expected<SaveData, LoadError> save_data;
  try {
    save_data = save_manager::LoadLatestSave();
  } catch (std::exception& e) {
    core::Log(std::format("Caught exception: {} while loading save.", e.what()),
              "BattleScene");
  }
  auto enemy_names = ListEnemyNames(enemies_);

  if (save_data.has_value()) {
    if (enemy_names.has_value()) {
      save_data->completion_markers.push_back(
          "Battle." +
          std::to_string(
              std::chrono::system_clock::now().time_since_epoch().count()) +
          "." + enemy_names.value());
    }

    if (enemy_names.has_value()) {
      save_data->log.NewLog(
          {.title = {{game::DegradationLevel::kLevel0,
                      "Battle against " + enemy_names.value()}},
           .description = {{game::DegradationLevel::kLevel0,
                            "No description."}},
           .timestamp = std::to_string(
               std::chrono::system_clock::now().time_since_epoch().count()),
           .texture = {}});
    }
    if (player_health_.health <= 0) {
      save_data->death_count++;
      player_health_.health = player_health_.max_health;
    }

    save_data->player_health = player_health_;
    save_data->defeated_enemy_uids.insert(save_data->defeated_enemy_uids.end(),
                                          defeated_enemies.begin(),
                                          defeated_enemies.end());
    save_data->acquired_skills = player_skills_.skills;
    save_manager::SaveGame(save_data.value(), save_data->log);
  }
}
