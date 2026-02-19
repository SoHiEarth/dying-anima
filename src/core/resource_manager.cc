#include "core/resource_manager.h"
#include <format>
#include <print>
#include <stdexcept>

void ResourceManager::Init() {
  texture_atlas = LoadTextureAtlas("assets/textures/texture.xml");
  shader_atlas = LoadShaderAtlas("assets/shaders/shader.xml");
  font_atlas = LoadFontAtlas("assets/fonts/font.xml");
}

TextureHandle ResourceManager::GetTexture(const std::string& tag) {
  if (auto it = texture_atlas.find(tag);it != texture_atlas.end()) {
    return it->second;
  }
  std::print("Texture with tag '{}' not found in atlas\n", tag);
  // Return util.notexture
  if (auto it = texture_atlas.find("util.notexture"); it != texture_atlas.end()) {
    return it->second;
  } else {
    throw std::runtime_error(
        "Fallback texture 'util.notexture' not found in atlas");
  }
}

ShaderHandle ResourceManager::GetShader(const std::string& tag) {
  if (auto it = shader_atlas.find(tag); it != shader_atlas.end()) {
    return it->second;
  }
  throw std::runtime_error(
      std::format("Shader with tag '{}' not found in atlas", tag));
}

FontHandle ResourceManager::GetFont(const std::string& tag) {
  if (auto it = font_atlas.find(tag); it != font_atlas.end()) {
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

void ResourceManager::Quit() {
  for (auto &[_, entry] : texture_atlas) {
    delete entry.texture;
  }
  texture_atlas.clear();
  for (auto &[_, entry] : shader_atlas) {
    delete entry.shader;
  }
  shader_atlas.clear();
  for (auto &[_, entry] : font_atlas) {
    delete entry.font;
  }
  font_atlas.clear();
}
