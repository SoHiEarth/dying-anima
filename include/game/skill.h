#ifndef GAME_SKILL_H
#define GAME_SKILL_H

#include <string>

struct Skill {
  std::string name;
  float damage;         // The amount of damage applied to the target
  float stamina_drain;  // The amount of stamina drained from the target
  float health_used;    // The amount of health drained from the user
  float stamina_used;   // The amount of stamina drained from the user
};

#endif  // GAME_SKILL_H
