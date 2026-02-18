#pragma once
#include "shader.h"
#include <glm/glm.hpp>
#include <map>

struct Character {
  unsigned int texture;
  glm::ivec2 size;
  glm::ivec2 bearing;
  unsigned int advance;
};

struct Font {
  unsigned int vertex_attrib;
  unsigned int vertex_buffer;
  std::map<char, Character> characters;
  Font(std::string_view font_path);
  int GetWidth(std::string_view text) const;
  int GetHeight(std::string_view text) const;
  void Render(std::string_view text, const glm::vec2 &position,
              const glm::vec3 &color, const Shader *shader,
              const glm::mat4 &projection) const;
};
