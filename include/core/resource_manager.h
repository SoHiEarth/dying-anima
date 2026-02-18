#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H
#include "../atlas.h"

namespace ResourceManager {
static TEXTURE_ATLAS texture_atlas;
static SHADER_ATLAS shader_atlas;
static FONT_ATLAS font_atlas;
void Init();
TextureHandle GetTexture(std::string_view tag);
ShaderHandle GetShader(std::string_view tag);
FontHandle GetFont(std::string_view tag);
void Quit();
} // namespace ResourceManager

#endif // RESOURCE_MANAGER_H
