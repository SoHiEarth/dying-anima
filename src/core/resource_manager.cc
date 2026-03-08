#include "core/resource_manager.h"

#include <format>
#include <print>
#include <stdexcept>

namespace ResourceManager {
TEXTURE_ATLAS texture_atlas;
SHADER_ATLAS shader_atlas;
FONT_ATLAS font_atlas;
}  // namespace ResourceManager

void ResourceManager::Init() {
  texture_atlas = LoadTextureAtlas("assets/textures/texture.xml");
  shader_atlas = LoadShaderAtlas("assets/shaders/shader.xml");
  font_atlas = LoadFontAtlas("assets/fonts/font.xml");
}

TextureHandle ResourceManager::GetTexture(std::string_view tag) {
  if (auto it = texture_atlas.find(std::string(tag));
      it != texture_atlas.end()) {
    return it->second;
  }
  std::print("Texture with tag '{}' not found in atlas\n", tag);
  // Return util.notexture
  if (auto it = texture_atlas.find("util.notexture");
      it != texture_atlas.end()) {
    return it->second;
  } else {
    throw std::runtime_error(
        "Fallback texture 'util.notexture' not found in atlas");
  }
}

ShaderHandle ResourceManager::GetShader(std::string_view tag) {
  if (auto it = shader_atlas.find(std::string(tag)); it != shader_atlas.end()) {
    return it->second;
  }
  throw std::runtime_error(
      std::format("Shader with tag '{}' not found in atlas", tag));
}

FontHandle ResourceManager::GetFont(std::string_view tag) {
  if (auto it = font_atlas.find(std::string(tag)); it != font_atlas.end()) {
    return it->second;
  }
  std::print("Font with tag '{}' not found in atlas\n", tag);
  // Return util.nofont
  if (auto it = font_atlas.find("util.nofont"); it != font_atlas.end()) {
    return it->second;
  } else {
    throw std::runtime_error("Fallback font 'util.nofont' not found in atlas");
  }
}

// Will destroy all existing textures. Reaquire textures again.
void ResourceManager::ReloadTextures() {
  texture_atlas.clear();
  texture_atlas = LoadTextureAtlas("assets/textures/texture.xml");
}

void ResourceManager::ReloadShaders() {
  shader_atlas.clear();
  shader_atlas = LoadShaderAtlas("assets/shaders/shader.xml");
}

void ResourceManager::ReloadFonts() {
  font_atlas.clear();
  font_atlas = LoadFontAtlas("assets/fonts/font.xml");
}

void ResourceManager::Quit() {
  texture_atlas.clear();
  shader_atlas.clear();
  font_atlas.clear();
}
