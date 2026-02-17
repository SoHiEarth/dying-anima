#pragma once
#include <map>
#include <string>

#include "texture.h"

struct FontAtlasEntry {
  std::string name;
  std::string file;
};

struct ShaderAtlasEntry {
  std::string name;
  std::string vertex_file;
  std::string fragment_file;
};

struct TextureAtlasEntry {
  std::string path;
  Texture *texture = nullptr;
};

std::map<std::string, FontAtlasEntry> LoadFontAtlas(std::string_view filename);
std::map<std::string, ShaderAtlasEntry>
LoadShaderAtlas(std::string_view filename);
std::map<std::string, TextureAtlasEntry>
LoadTextureAtlas(std::string_view filename);