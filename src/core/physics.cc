#include "core/physics.h"

#include <box2d/box2d.h>
b2WorldId physics::world;

void physics::Init(const glm::vec2& gravity) {
  b2WorldDef world_def = b2DefaultWorldDef();
  world_def.gravity = {.x = gravity.x, .y = gravity.y};
  world = b2CreateWorld(&world_def);
}

void physics::SetGravity(const glm::vec2& gravity) {
  b2World_SetGravity(world, {gravity.x, gravity.y});
}

b2BodyId physics::CreateChainBody(const std::vector<glm::vec2>& vertices) {
  auto body_def = b2DefaultBodyDef();
  body_def.type = b2_staticBody;
  auto body = b2CreateBody(world, &body_def);
  std::vector<b2Vec2> b2_vertices;
  b2_vertices.reserve(vertices.size());
  for (const auto& vertex : vertices) {
    b2_vertices.push_back({vertex.x, vertex.y});
  }

  auto chain_def = b2DefaultChainDef();
  chain_def.points = b2_vertices.data();
  chain_def.count = static_cast<int>(b2_vertices.size());
  b2CreateChain(body, &chain_def);
  return body;
}

PhysicsBody physics::CreateBody(Transform transform, bool is_dynamic) {
  PhysicsBody physics_body = CreateBody(transform.position, transform.scale,
                                        transform.rotation, is_dynamic);
  return physics_body;
}

PhysicsBody physics::CreateBody(const glm::vec2& position,
                                const glm::vec2& scale, float angle,
                                bool is_dynamic) {
  PhysicsBody physics_body;
  auto body_def = b2DefaultBodyDef();
  if (is_dynamic) {
    body_def.type = b2_dynamicBody;
  } else {
    body_def.type = b2_staticBody;
  }
  body_def.position = {.x = position.x, .y = position.y};
  body_def.rotation = b2MakeRot(angle);
  body_def.fixedRotation = true;
  auto body = b2CreateBody(world, &body_def);
  auto shape = b2MakeBox((scale.x / 2.0F), (scale.y / 2.0F));
  auto shape_def = b2DefaultShapeDef();
  shape_def.density = 1.0F;
  shape_def.material.friction = 0.0F;
  physics_body.shape = b2CreatePolygonShape(body, &shape_def, &shape);
  physics_body.body = body;
  physics_body.is_dynamic = is_dynamic;
  return physics_body;
}

void physics::SyncPosition(b2BodyId body, glm::vec2& position) {
  b2Vec2 b2_position = b2Body_GetPosition(body);
  position.x = b2_position.x;
  position.y = b2_position.y;
}

void physics::SyncTransform(b2BodyId body, Transform& transform) {
  b2Vec2 b2_position = b2Body_GetPosition(body);
  transform.position = {b2_position.x, b2_position.y};
  transform.rotation = b2Rot_GetAngle(b2Body_GetRotation(body));
}

bool physics::WorldValid() { return b2World_IsValid(world); }

bool physics::IsColliding(PhysicsBody body1, PhysicsBody body2) {
  int body_contact_capacity = b2Body_GetContactCapacity(body1.body);
  std::vector<b2ContactData> contacts(body_contact_capacity);
  int body_contact_count =
      b2Body_GetContactData(body1.body, contacts.data(), body_contact_capacity);
  for (int i = 0; i < body_contact_count; ++i) {
    b2ContactData contact = contacts[i];
    if (contact.shapeIdA.index1 == body2.shape.index1 ||
        contact.shapeIdB.index1 == body2.shape.index1) {
      return true;
    }
  }
  return false;
}

void physics::Quit() {
  if (b2World_IsValid(world)) {
    b2DestroyWorld(world);
  }
}
