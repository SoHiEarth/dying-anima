#ifndef BATTLE_H
#define BATTLE_H

#include <utility>

#include "core/scene.h"
#include "game/enemy.h"
#include "game/player.h"

struct BattleScene : public Scene {
  std::vector<Enemy> enemies_;
  PlayerSkills player_skills_;
  Health player_health_;

 public:
  BattleScene(SceneManager& manager, std::vector<Enemy> enemies,
              PlayerSkills& player_skills_, Health& player_health_)
      : Scene(manager),
        enemies_(std::move(enemies)),
        player_skills_(player_skills_),
        player_health_(player_health_) {}
  std::string Name() override { return "BattleScene"; };
  void Init() override;
  void Quit() override;
  void Update(double dt) override;
  void Render(GameWindow& window) override;
  bool IsTransparent() override { return false; };
};

#endif  // BATTLE_H
