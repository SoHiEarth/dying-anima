#ifndef PHYSICS_H
#define PHYSICS_H

#include "transform.h"
#include <box2d/box2d.h>
#include <glm/glm.hpp>

struct PhysicsBody {
  b2BodyId body;
};

namespace physics {
b2WorldId CreateWorld(const glm::vec2 &gravity);
void SetGravity(b2WorldId world, const glm::vec2 &gravity);
b2BodyId CreateBody(b2WorldId world, const glm::vec2 &position,
                    const glm::vec2 &scale, float angle, bool is_dynamic);
b2BodyId CreateBody(b2WorldId world, Transform transform, bool is_dynamic);
void SyncPosition(b2BodyId body, glm::vec2 &position);
void SyncTransform(b2BodyId body, Transform &transform);

void DestroyWorld(b2WorldId &world);
} // namespace physics

#endif // PHYSICS_H
