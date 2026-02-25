#ifndef SPRITE_H
#define SPRITE_H

#include <string>
#include "texture.h"
struct Sprite {
  std::string texture_tag;
  Texture* texture;
  Texture* normal;
};

#endif // SPRITE_H
