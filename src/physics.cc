#include "core/physics.h"
#include <box2d/box2d.h>
b2WorldId physics::world;

void physics::Init(const glm::vec2 &gravity) {
  b2WorldDef world_def = b2DefaultWorldDef();
  world_def.gravity = {gravity.x, gravity.y};
  world = b2CreateWorld(&world_def);
}

void physics::SetGravity(const glm::vec2 &gravity) {
  b2World_SetGravity(world, {gravity.x, gravity.y});
}

#define GRACE_SCALE 0.025f
#define SCALE_MULTIPLIER 1.0f - GRACE_SCALE
b2BodyId physics::CreateBody(const glm::vec2 &position,
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
  auto shape = b2MakeBox(
    (scale.x / 2.0f) * SCALE_MULTIPLIER,
    (scale.y / 2.0f) * SCALE_MULTIPLIER);
  auto shape_def = b2DefaultShapeDef();
  shape_def.density = 1.0F;
  shape_def.material.friction = 0.3F;
  b2CreatePolygonShape(body, &shape_def, &shape);
  return body;
}

b2BodyId physics::CreateBody(Transform transform, bool is_dynamic) {
  return CreateBody(transform.position, transform.scale,
                           transform.rotation, is_dynamic);
}

b2BodyId physics::CreateChainBody(const std::vector<glm::vec2> &vertices) {
  auto body_def = b2DefaultBodyDef();
  body_def.type = b2_staticBody;
  auto body = b2CreateBody(world, &body_def);
  std::vector<b2Vec2> b2_vertices;
  for (const auto& vertex : vertices) {
    b2_vertices.push_back({vertex.x, vertex.y});
  }
  
  auto chain_def = b2DefaultChainDef();
  chain_def.points = b2_vertices.data();
  chain_def.count = static_cast<int>(b2_vertices.size());
  b2CreateChain(body, &chain_def);
  return body;
}

PhysicsBody physics::CreatePhysicsBody(Transform transform, bool is_dynamic) {
  PhysicsBody physics_body;
  physics_body.body = CreateBody(transform, is_dynamic);
  physics_body.is_dynamic = is_dynamic;
  return physics_body;
}

PhysicsBody physics::CreatePhysicsBody(const glm::vec2 &position,
                                       const glm::vec2 &scale, float angle,
                                       bool is_dynamic) {
  PhysicsBody physics_body;
  physics_body.body = CreateBody(position, scale, angle, is_dynamic);
  physics_body.is_dynamic = is_dynamic;
  return physics_body;
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

bool physics::WorldValid() { return b2World_IsValid(world); }

void physics::Quit() {
  if (b2World_IsValid(world)) {
    b2DestroyWorld(world);
  }
}
