#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H
#include "../atlas.h"

namespace ResourceManager {
static TEXTURE_ATLAS texture_atlas;
static SHADER_ATLAS shader_atlas;
static FONT_ATLAS font_atlas;
void Init();
TextureHandle GetTexture(const std::string& tag);
ShaderHandle GetShader(const std::string& tag);
FontHandle GetFont(const std::string& tag);
void Quit();
} // namespace ResourceManager

#endif // RESOURCE_MANAGER_H
