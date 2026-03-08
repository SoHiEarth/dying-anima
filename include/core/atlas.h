#pragma once
#include <map>
#include <memory>
#include <string>

#include "core/font.h"
#include "core/shader.h"
#include "core/texture.h"

struct FontHandle {
  std::string name;
  std::string file;
  std::shared_ptr<Font> font;
};

struct ShaderHandle {
  std::string name;
  std::string vertex_file;
  std::string fragment_file;
  std::shared_ptr<Shader> shader;
};

struct TextureHandle {
  std::string path;
  std::shared_ptr<Texture> texture;
};

#define FONT_ATLAS std::map<std::string, FontHandle>
#define SHADER_ATLAS std::map<std::string, ShaderHandle>
#define TEXTURE_ATLAS std::map<std::string, TextureHandle>

FONT_ATLAS LoadFontAtlas(std::string_view filename);
SHADER_ATLAS LoadShaderAtlas(std::string_view filename);
TEXTURE_ATLAS LoadTextureAtlas(std::string_view filename);
