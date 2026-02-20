#ifndef SPRITE_H
#define SPRITE_H

#include <string>
#include "texture.h"
struct Sprite {
  std::string texture_tag;
  Texture* texture;
};

#endif // SPRITE_H
