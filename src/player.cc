#include "player.h"

bool Player::IsOnGround() {
  int capacity = b2Body_GetContactCapacity(body);
  if (capacity <= 0) {
    return false;
  }
  std::vector<b2ContactData> contacts(capacity);
  int count = b2Body_GetContactData(body, contacts.data(), capacity);
  for (int i = 0; i < count; ++i) {
    b2ContactData &cd = contacts[i];
    if (cd.manifold.normal.y <= -0.9f) {
      return true;
    }
  }
  return false;
}