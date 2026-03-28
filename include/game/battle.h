#ifndef BATTLE_H
#define BATTLE_H

#include "core/scene.h"
#include "game/enemy.h"

struct BattleScene : public Scene {
  std::vector<Enemy> enemies_;

 public:
	 BattleScene(SceneManager& manager, std::vector<Enemy> enemies)
      : Scene(manager), enemies_(std::move(enemies)) {}
   std::string Name() override { return "BattleScene"; };
   void Init() override;
   void Quit() override;
   void Update(double dt) override;
   void Render(GameWindow& window) override;
   bool IsTransparent() override { return false; };
   entt::registry registry;
};

#endif  // BATTLE_H