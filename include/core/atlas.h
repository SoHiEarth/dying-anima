#ifndef CORE_ATLAS_H
#define CORE_ATLAS_H

#include <map>
#include <memory>
#include <string>

#include "core/font.h"
#include "core/shader.h"
#include "core/texture.h"

struct FontHandle {
  std::string tag;
  std::string name;
  std::string file;
  std::shared_ptr<Font> font;
};

struct ShaderHandle {
  std::string tag;
  std::string name;
  std::string vertex_file;
  std::string fragment_file;
  std::shared_ptr<Shader> shader;
};

struct TextureHandle {
  std::string tag;
  std::string path;
  std::shared_ptr<Texture> texture;
};

#define FONT_ATLAS std::map<std::string, FontHandle>
#define SHADER_ATLAS std::map<std::string, ShaderHandle>
#define TEXTURE_ATLAS std::map<std::string, TextureHandle>

FONT_ATLAS LoadFontAtlas(const std::filesystem::path& filename);
SHADER_ATLAS LoadShaderAtlas(const std::filesystem::path& filename);
TEXTURE_ATLAS LoadTextureAtlas(const std::filesystem::path& filename);

#endif  // CORE_ATLAS_H
