#include "core/resource_manager.h"
#include "core/log.h"
#include "core/path_resolve.h"
#include <format>
#include <print>
#include <stdexcept>

namespace resource_manager {
TEXTURE_ATLAS texture_atlas;
SHADER_ATLAS shader_atlas;
FONT_ATLAS font_atlas;
}  // namespace resource_manager

void resource_manager::Init() {
  texture_atlas = LoadTextureAtlas((core::path::GetAssetPath() / "textures/texture.xml").string());
  shader_atlas = LoadShaderAtlas((core::path::GetAssetPath() / "shaders/shader.xml").string());
  font_atlas = LoadFontAtlas((core::path::GetAssetPath() / "fonts/font.xml").string());
}

TextureHandle resource_manager::GetTexture(std::string_view tag) {
  if (auto it = texture_atlas.find(std::string(tag));
      it != texture_atlas.end()) {
    return it->second;
  }
  core::Log(std::format("Texture with tag '{}' not found in atlas", tag), "Resource Manager");
  // Return util.notexture
  if (auto it = texture_atlas.find("util.notexture");
      it != texture_atlas.end()) {
    return it->second;
  }
  throw core::Error("Fallback texture 'util.notexture' not found", "Resource Manager");
}

ShaderHandle resource_manager::GetShader(std::string_view tag) {
  if (auto it = shader_atlas.find(std::string(tag)); it != shader_atlas.end()) {
    return it->second;
  }
  throw core::Error(std::format("Shader with tag '{}' not found in atlas", tag), "Resource Manager");
}

FontHandle resource_manager::GetFont(std::string_view tag) {
  if (auto it = font_atlas.find(std::string(tag)); it != font_atlas.end()) {
    return it->second;
  }

  core::Log(std::format("Font with tag '{}' not found in atlas", tag), "Resource Manager");
  // Return util.nofont
  if (auto it = font_atlas.find("util.nofont"); it != font_atlas.end()) {
    return it->second;
  }
  throw core::Error("Fallback font 'util.nofont' not found in atlas", "Resource Manager");
}

// Will destroy all existing textures. Reaquire textures again.
void resource_manager::ReloadTextures() {
  texture_atlas.clear();
  texture_atlas = LoadTextureAtlas("assets/textures/texture.xml");
}

void resource_manager::ReloadShaders() {
  shader_atlas.clear();
  shader_atlas = LoadShaderAtlas("assets/shaders/shader.xml");
}

void resource_manager::ReloadFonts() {
  font_atlas.clear();
  font_atlas = LoadFontAtlas("assets/fonts/font.xml");
}

void resource_manager::Quit() {
  texture_atlas.clear();
  shader_atlas.clear();
  font_atlas.clear();
}
