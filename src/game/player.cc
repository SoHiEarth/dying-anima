#include "game/player.h"
#include <vector>

/*
bool IsOnGround(b2BodyId body) {
  int capacity = b2Body_GetContactCapacity(body);
  if (capacity <= 0) {
    return false;
  }
  std::vector<b2ContactData> contacts(capacity);
  int count = b2Body_GetContactData(body, contacts.data(), capacity);
  if (count > capacity) {
    count = capacity;
  }
  
  for (int i = 0; i < count; ++i) {
    b2ContactData &cd = contacts[i];
    if (cd.manifold.normal.y <= -0.707f) {
      return true;
    }
  }
  return false;
}
*/

// Testing new implementation
bool IsOnGround(b2BodyId body) {
  auto velocity = b2Body_GetLinearVelocity(body);
  if (velocity.y == 0) {
    return true;
  }
  return false;
}