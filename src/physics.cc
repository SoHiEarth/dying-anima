#include "physics.h"

b2WorldId physics::CreatePhysicsWorld(const glm::vec2 &gravity) {
  b2WorldDef world_def = b2DefaultWorldDef();
  world_def.gravity = {gravity.x, gravity.y};
  return b2CreateWorld(&world_def);
}

b2BodyId physics::CreatePhysicsBody(b2WorldId world, const glm::vec2 &position,
                           const glm::vec2 &scale, float angle,
                           bool is_dynamic) {
  auto body_def = b2DefaultBodyDef();
  if (is_dynamic) {
    body_def.type = b2_dynamicBody;
  } else {
    body_def.type = b2_staticBody;
  }
  body_def.position = {position.x, position.y};
  body_def.rotation = b2MakeRot(angle);
  body_def.fixedRotation = true;
  auto body = b2CreateBody(world, &body_def);
  auto shape = b2MakeBox(scale.x / 2.0f, scale.y / 2.0f);
  auto shape_def = b2DefaultShapeDef();
  shape_def.density = 1.0F;
  shape_def.material.friction = 0.3F;
  b2CreatePolygonShape(body, &shape_def, &shape);
  return body;
}

void physics::SyncPosition(b2BodyId body, glm::vec2 &position) {
  b2Vec2 b2_position = b2Body_GetPosition(body);
  position.x = b2_position.x;
  position.y = b2_position.y;
}

void physics::DestroyPhysicsWorld(b2WorldId &world) {
  if (b2World_IsValid(world)) {
    b2DestroyWorld(world);
    world = b2WorldId{};
  }
}
