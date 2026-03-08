#ifndef PHYSICS_H
#define PHYSICS_H

#include <box2d/box2d.h>

#include <glm/glm.hpp>

#include "core/transform.h"

struct PhysicsBody {
  b2BodyId body;
  bool is_dynamic = false;
  bool is_chained = false;
};

namespace physics {
extern b2WorldId world;
bool WorldValid();
void Init(const glm::vec2& gravity);
void SetGravity(const glm::vec2& gravity);
b2BodyId CreateBody(const glm::vec2& position, const glm::vec2& scale,
                    float angle, bool is_dynamic);
b2BodyId CreateBody(Transform transform, bool is_dynamic);
b2BodyId CreateChainBody(const std::vector<glm::vec2>& vertices);
PhysicsBody CreatePhysicsBody(Transform transform, bool is_dynamic);
PhysicsBody CreatePhysicsBody(const glm::vec2& position, const glm::vec2& scale,
                              float angle, bool is_dynamic);
void SyncPosition(b2BodyId body, glm::vec2& position);
void SyncTransform(b2BodyId body, Transform& transform);
void Quit();
}  // namespace physics

#endif  // PHYSICS_H
