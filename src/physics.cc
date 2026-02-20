#include "physics.h"

b2WorldId physics::CreateWorld(const glm::vec2 &gravity) {
  b2WorldDef world_def = b2DefaultWorldDef();
  world_def.gravity = {gravity.x, gravity.y};
  return b2CreateWorld(&world_def);
}

void physics::SetGravity(b2WorldId world, const glm::vec2 &gravity) {
  b2World_SetGravity(world, {gravity.x, gravity.y});
}

b2BodyId physics::CreateBody(b2WorldId world, const glm::vec2 &position,
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

b2BodyId physics::CreateBody(b2WorldId world, Transform transform,
                                    bool is_dynamic) {
  return CreateBody(world, transform.position, transform.scale,
                           transform.rotation, is_dynamic);
}

void physics::SyncPosition(b2BodyId body, glm::vec2 &position) {
  b2Vec2 b2_position = b2Body_GetPosition(body);
  position.x = b2_position.x;
  position.y = b2_position.y;
}

void physics::SyncTransform(b2BodyId body, Transform &transform) {
  b2Vec2 b2_position = b2Body_GetPosition(body);
  transform.position = { b2_position.x, b2_position.y };
  transform.rotation = b2Rot_GetAngle(b2Body_GetRotation(body));
}

void physics::DestroyWorld(b2WorldId &world) {
  if (b2World_IsValid(world)) {
    b2DestroyWorld(world);
    world = b2WorldId{};
  }
}
