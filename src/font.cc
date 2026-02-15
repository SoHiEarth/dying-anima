#include <ft2build.h>
#include <glad/glad.h>

#include <glm/gtc/type_ptr.hpp>
#include <stdexcept>
#include <string>

#include "shader.h"
#include FT_FREETYPE_H
#include "font.h"

Font::Font(std::string_view font_path, unsigned int font_size) {
  glGenVertexArrays(1, &vertex_attrib);
  glGenBuffers(1, &vertex_buffer);
  glBindVertexArray(vertex_attrib);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr,
               GL_DYNAMIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                        (void*)(2 * sizeof(float)));

  FT_Library ft;
  if (FT_Init_FreeType(&ft)) {
    throw std::runtime_error("Could not init FreeType Library");
  }

  FT_Face face;
  if (FT_New_Face(ft, font_path.data(), 0, &face)) {
    FT_Done_FreeType(ft);
    throw std::runtime_error("Failed to load font: " + std::string(font_path));
  }

  FT_Set_Pixel_Sizes(face, 0, font_size);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  for (unsigned char c = 0; c < 128; c++) {
    if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
      throw std::runtime_error("Failed to load Glyph");
    }
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width,
                 face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE,
                 face->glyph->bitmap.buffer);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    Character character = {
        texture,
        glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
        glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
        static_cast<unsigned int>(face->glyph->advance.x)};
    characters.insert(std::pair<char, Character>(c, character));
  }
  glBindTexture(GL_TEXTURE_2D, 0);
  FT_Done_Face(face);
  FT_Done_FreeType(ft);
}

int Font::GetWidth(std::string_view text, float scale) const {
  int width = 0;
  for (const char& c : text) {
    const auto& ch = characters.at(c);
    width += (ch.advance >> 6) * (int)scale;
  }
  return width;
}

int Font::GetHeight(std::string_view text, float scale) const {
  int height = 0;
  for (const char& c : text) {
    const auto& ch = characters.at(c);
    int h = ch.size.y * (int)scale;
    if (h > height) {
      height = h;
    }
  }
  return height;
}

void Font::Render(std::string_view text, const glm::vec2& position,
                  const float scale, const glm::vec3& color,
                  const Shader& shader, const glm::mat4& projection) const {
  shader.Use();
  shader.SetUniform("color", color);
  shader.SetUniform("projection", projection);
  glActiveTexture(GL_TEXTURE0);
  glBindVertexArray(vertex_attrib);

  float x_pos = position.x;
  float y_pos = position.y;
  for (const char& c : text) {
    const auto& ch = characters.at(c);
    float xpos = x_pos + ch.bearing.x * scale;
    float ypos = y_pos - (ch.size.y - ch.bearing.y) * scale;
    float w = ch.size.x * scale;
    float h = ch.size.y * scale;

    float vertices[6][4] = {
        {xpos, ypos + h, 0.0f, 0.0f},    {xpos, ypos, 0.0f, 1.0f},
        {xpos + w, ypos, 1.0f, 1.0f},

        {xpos, ypos + h, 0.0f, 0.0f},    {xpos + w, ypos, 1.0f, 1.0f},
        {xpos + w, ypos + h, 1.0f, 0.0f}};
    glBindTexture(GL_TEXTURE_2D, ch.texture);
    glBindVertexArray(vertex_attrib);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    x_pos += (ch.advance >> 6) * scale;
  }
  glBindVertexArray(0);
  glBindTexture(GL_TEXTURE_2D, 0);
}