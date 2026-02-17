#ifndef PHYSICS_H
#define PHYSICS_H

#include <box2d/box2d.h>
#include <glm/glm.hpp>

struct PhysicsBody {
  b2BodyId body;
};

namespace physics {
b2WorldId CreatePhysicsWorld(const glm::vec2 &gravity);
b2BodyId CreatePhysicsBody(b2WorldId world, const glm::vec2 &position,
                           const glm::vec2 &scale, float angle,
                           bool is_dynamic);
void SyncPosition(b2BodyId body, glm::vec2 &position);

void DestroyPhysicsWorld(b2WorldId &world);
} // namespace physics

#endif // PHYSICS_H
