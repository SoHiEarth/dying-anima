#ifndef SPRITE_H
#define SPRITE_H

#include <string>
#include "texture.h"
struct Sprite {
  std::string texture_tag;
  std::shared_ptr<Texture> texture;
  std::shared_ptr<Texture> normal;
};

#endif // SPRITE_H
