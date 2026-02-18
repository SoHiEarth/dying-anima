#pragma once
#include "font.h"
#include "shader.h"
#include "texture.h"
#include <map>
#include <string>

struct FontHandle {
  std::string name;
  std::string file;
  Font *font = nullptr;
};

struct ShaderHandle {
  std::string name;
  std::string vertex_file;
  std::string fragment_file;
  Shader *shader = nullptr;
};

struct TextureHandle {
  std::string path;
  Texture *texture = nullptr;
};

#define FONT_ATLAS std::map<std::string, FontHandle>
#define SHADER_ATLAS std::map<std::string, ShaderHandle>
#define TEXTURE_ATLAS std::map<std::string, TextureHandle>

FONT_ATLAS LoadFontAtlas(std::string_view filename);
SHADER_ATLAS LoadShaderAtlas(std::string_view filename);
TEXTURE_ATLAS LoadTextureAtlas(std::string_view filename);
