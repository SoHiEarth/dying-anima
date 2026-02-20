#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H
#include "../atlas.h"

namespace ResourceManager {
extern TEXTURE_ATLAS texture_atlas;
extern SHADER_ATLAS shader_atlas;
extern FONT_ATLAS font_atlas;
void Init();
TextureHandle GetTexture(const std::string& tag);
ShaderHandle GetShader(const std::string& tag);
FontHandle GetFont(const std::string& tag);
void Quit();
} // namespace ResourceManager

#endif // RESOURCE_MANAGER_H
