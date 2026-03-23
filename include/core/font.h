#pragma once
#include <glm/glm.hpp>
#include <map>
#include <memory>

#include "core/shader.h"

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
  explicit Font(std::string_view font_path);
  ~Font();
  int GetWidth(std::string_view text) const;
  int GetHeight(std::string_view text) const;
  float GetWidthScale(std::string_view text, float height) const;
  // For compatibility, scale is glm::vec2(1.0f)
  void Render(std::string_view text, const glm::vec2& position,
              const glm::vec3& color,
              const std::shared_ptr<Shader>& shader) const;
  void Render(std::string_view text, const glm::vec2& position,
              const glm::vec2& scale, const glm::vec3& color,
              const std::shared_ptr<Shader>& shader) const;
  void RenderUI(std::string_view text, const glm::vec2& position,
                const glm::vec2& scale, const glm::vec3& color,
                const std::shared_ptr<Shader>& shader) const;
  // Render with exact pixel height (width scales proportionally)
  void RenderUIAtHeight(std::string_view text, const glm::vec2& position,
                        float height_pixels, const glm::vec3& color,
                        const std::shared_ptr<Shader>& shader) const;
};
