#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H
#include "core/atlas.h"
#define IMGUI_TEXTURE_FLIP ImVec2(0, 1), ImVec2(1, 0)

namespace resource_manager {
extern TEXTURE_ATLAS texture_atlas;
extern SHADER_ATLAS shader_atlas;
extern FONT_ATLAS font_atlas;
void Init();
TextureHandle GetTexture(std::string_view tag);
ShaderHandle GetShader(std::string_view tag);
FontHandle GetFont(std::string_view tag);
void ReloadTextures();
void ReloadShaders();
void ReloadFonts();
void Quit();
}  // namespace resource_manager

#endif  // RESOURCE_MANAGER_H
