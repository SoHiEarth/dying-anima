#include "core/resource_manager.h"
#include <format>
#include <print>
#include <stdexcept>

namespace ResourceManager {
  TEXTURE_ATLAS texture_atlas;
  SHADER_ATLAS shader_atlas;
  FONT_ATLAS font_atlas;
}

void ResourceManager::Init() {
  texture_atlas = LoadTextureAtlas("assets/textures/texture.xml");
  shader_atlas = LoadShaderAtlas("assets/shaders/shader.xml");
  font_atlas = LoadFontAtlas("assets/fonts/font.xml");
}

TextureHandle ResourceManager::GetTexture(std::string_view tag) {
  if (texture_atlas.contains(std::string(tag))) {
    return texture_atlas.at(std::string(tag));
  }
  std::print("Texture with tag '{}' not found in atlas\n", tag);
  // Return util.notexture
  if (texture_atlas.contains("util.notexture")) {
    return texture_atlas.at("util.notexture");
  } else {
    throw std::runtime_error(
        "Fallback texture 'util.notexture' not found in atlas");
  }
}

ShaderHandle ResourceManager::GetShader(std::string_view tag) {
  if (shader_atlas.contains(std::string(tag))) {
    return shader_atlas.at(std::string(tag));
  }
  throw std::runtime_error(
      std::format("Shader with tag '{}' not found in atlas", tag));
}

FontHandle ResourceManager::GetFont(std::string_view tag) {
  if (font_atlas.contains(std::string(tag))) {
    return font_atlas.at(std::string(tag));
  }
  std::print("Font with tag '{}' not found in atlas\n", tag);
  // Return util.nofont
  if (font_atlas.contains("util.nofont")) {
    return font_atlas.at("util.nofont");
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
