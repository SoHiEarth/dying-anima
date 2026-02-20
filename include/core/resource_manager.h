#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H
#include "../atlas.h"

namespace ResourceManager {
extern TEXTURE_ATLAS texture_atlas;
extern SHADER_ATLAS shader_atlas;
extern FONT_ATLAS font_atlas;
void Init();
TextureHandle GetTexture(std::string_view tag);
ShaderHandle GetShader(std::string_view tag);
FontHandle GetFont(std::string_view tag);
void Quit();
} // namespace ResourceManager

#endif // RESOURCE_MANAGER_H
