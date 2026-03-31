#include <ft2build.h>
#include <glad/glad.h>

#include <algorithm>
#include <glm/gtc/type_ptr.hpp>
#include <stdexcept>
#include <string>

#include "core/shader.h"
#include FT_FREETYPE_H
#include "core/camera.h"
#include "core/font.h"
#include "core/window.h"

namespace {
glm::mat4 last_projection, last_view, last_vp;
}

Font::Font(std::string_view font_path) {
  auto dpi = 96.0F;
  glGenVertexArrays(1, &vertex_attrib);
  glGenBuffers(1, &vertex_buffer);
  glBindVertexArray(vertex_attrib);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 5, nullptr,
               GL_DYNAMIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), nullptr);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        reinterpret_cast<void*>(3 * sizeof(float)));

  FT_Library ft;
  if (FT_Init_FreeType(&ft)) {
    throw std::runtime_error("Could not init FreeType Library");
  }

  FT_Face face;
  if (static_cast<bool>(
          FT_New_Face(ft, std::string(font_path).c_str(), 0, &face))) {
    FT_Done_FreeType(ft);
    throw std::runtime_error("Failed to load font: " + std::string(font_path));
  }

  FT_Set_Pixel_Sizes(face, 0, static_cast<unsigned int>(dpi));
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
        .texture = texture,
        .size = glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
        .bearing =
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
        .advance = static_cast<unsigned int>(face->glyph->advance.x)};
    characters.insert(std::pair<char, Character>(c, character));
  }
  glBindTexture(GL_TEXTURE_2D, 0);
  FT_Done_Face(face);
  FT_Done_FreeType(ft);
}

Font::~Font() {
  for (const auto& [_, character] : characters) {
    glDeleteTextures(1, &character.texture);
  }
  glDeleteVertexArrays(1, &vertex_attrib);
  glDeleteBuffers(1, &vertex_buffer);
}

int Font::GetWidth(std::string_view text) const {
  int width = 0;
  for (const char& c : text) {
    const auto& ch = characters.at(c);
    width += (ch.advance >> 6);
  }
  return width;
}

int Font::GetHeight(std::string_view text) const {
  int height = 0;
  for (const auto& c : text) {
    const auto& ch = characters.at(c);
    int h = ch.size.y;
    height = std::max(h, height);
  }
  return height;
}

void Font::Render(std::string_view text, const glm::vec2& position,
                  const glm::vec3& color,
                  const std::shared_ptr<Shader>& shader) const {
  Render(text, position, glm::vec2(1.0F), color, shader);
}

void Font::Render(std::string_view text, const glm::vec2& position,
                  const glm::vec2& scale, const glm::vec3& color,
                  const std::shared_ptr<Shader>& shader) const {
  shader->Use();
  shader->SetUniform("color", color);
  if (last_projection != GetGameWindow().GetProjection()) {
    last_projection = GetGameWindow().GetProjection();
    last_vp = last_projection * GetCamera().GetView();
  }
  if (last_view != GetCamera().GetView()) {
    last_view = GetCamera().GetView();
    last_vp = GetGameWindow().GetProjection() * last_view;
  }

  shader->SetUniform(
      "mvp",
      last_vp * glm::translate(glm::mat4(1.0F), glm::vec3(position, 0.0F)));
  glActiveTexture(GL_TEXTURE0);
  glBindVertexArray(vertex_attrib);
  float total_width = GetWidth(text) * scale.x;
  float total_height = GetHeight(text) * scale.y;
  float x_pos = -total_width / 2.0F;
  float y_pos = -total_height / 2.0F;
  for (const char& c : text) {
    const auto& ch = characters.at(c);
    float xpos = x_pos + (ch.bearing.x * scale.x);
    float ypos = y_pos - ((ch.size.y - ch.bearing.y) * scale.y);
    float w = ch.size.x * scale.x;
    float h = ch.size.y * scale.y;

    float vertices[6][5] = {{xpos, ypos + h, 0.0F, 0.0F, 0.0F},
                            {xpos, ypos, 0.0F, 0.0F, 1.0F},
                            {xpos + w, ypos, 0.0F, 1.0F, 1.0F},
                            {xpos, ypos + h, 0.0F, 0.0F, 0.0F},
                            {xpos + w, ypos, 0.0F, 1.0F, 1.0F},
                            {xpos + w, ypos + h, 0.0F, 1.0F, 0.0F}};
    glBindTexture(GL_TEXTURE_2D, ch.texture);
    glBindVertexArray(vertex_attrib);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    x_pos += (ch.advance >> 6) * scale.x;
  }
  glBindVertexArray(0);
  glBindTexture(GL_TEXTURE_2D, 0);
}

void Font::RenderUI(std::string_view text, const glm::vec2& position,
                    const glm::vec2& scale, const glm::vec3& color,
                    const std::shared_ptr<Shader>& shader) const {
  shader->Use();
  shader->SetUniform("color", color);
  if (last_projection != GetGameWindow().GetProjection()) {
    last_projection = GetGameWindow().GetProjection();
    last_vp = last_projection * GetCamera().GetView();
  }
  if (last_view != GetCamera().GetView()) {
    last_view = GetCamera().GetView();
    last_vp = GetGameWindow().GetProjection() * last_view;
  }

  shader->SetUniform(
      "mvp",
      last_vp * glm::translate(glm::mat4(1.0F), glm::vec3(position, 0.0F)));
  glActiveTexture(GL_TEXTURE0);
  glBindVertexArray(vertex_attrib);
  float x_pos = 0;
  float y_pos = 0;
  for (const char& c : text) {
    const auto& ch = characters.at(c);
    float xpos = x_pos + (ch.bearing.x * scale.x);
    float ypos = y_pos - ((ch.size.y - ch.bearing.y) * scale.y);
    float w = ch.size.x * scale.x;
    float h = ch.size.y * scale.y;

    float vertices[6][5] = {{xpos, ypos + h, 0.0F, 0.0F, 0.0F},
                            {xpos, ypos, 0.0F, 0.0F, 1.0F},
                            {xpos + w, ypos, 0.0F, 1.0F, 1.0F},
                            {xpos, ypos + h, 0.0F, 0.0F, 0.0F},
                            {xpos + w, ypos, 0.0F, 1.0F, 1.0F},
                            {xpos + w, ypos + h, 0.0F, 1.0F, 0.0F}};
    glBindTexture(GL_TEXTURE_2D, ch.texture);
    glBindVertexArray(vertex_attrib);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    x_pos += (ch.advance >> 6) * scale.x;
  }
  glBindVertexArray(0);
  glBindTexture(GL_TEXTURE_2D, 0);
}

float Font::GetWidthScale(std::string_view text, float height) const {
  auto natural_height = static_cast<float>(GetHeight(text));
  if (natural_height == 0.0F) return 0.0F;
  return height / natural_height;
}

void Font::RenderUIAtHeight(std::string_view text, const glm::vec2& position,
                            float height_pixels, const glm::vec3& color,
                            const std::shared_ptr<Shader>& shader) const {
  RenderUI(text, position, glm::vec2(GetWidthScale(text, height_pixels)), color,
           shader);
}
