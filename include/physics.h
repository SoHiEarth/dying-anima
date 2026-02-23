#ifndef PHYSICS_H
#define PHYSICS_H

#include "transform.h"
#include <box2d/box2d.h>
#include <glm/glm.hpp>

struct PhysicsBody {
  b2BodyId body;
};

namespace physics {
extern b2WorldId world;
void Init(const glm::vec2 &gravity);
void SetGravity(const glm::vec2 &gravity);
b2BodyId CreateBody(const glm::vec2 &position,
                    const glm::vec2 &scale, float angle, bool is_dynamic);
b2BodyId CreateBody(Transform transform, bool is_dynamic);
void SyncPosition(b2BodyId body, glm::vec2 &position);
void SyncTransform(b2BodyId body, Transform &transform);
void Quit();
} // namespace physics

#endif // PHYSICS_H
