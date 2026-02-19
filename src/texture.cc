#include "texture.h"

#include <glad/glad.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <print>
#include <string>

#include "camera.h"
#include "shader.h"
#include "window.h"
#include "core/quad.h"

Texture::Texture(std::string_view path) {
  this->path = std::string(path);
  stbi_set_flip_vertically_on_load(true);
  unsigned char *data =
      stbi_load(std::string(path).c_str(), &width, &height, &channels, 0);
  if (data) {
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);
  } else {
    std::print("Failed to load texture: {}\n", path);
    id = 0;
    width = height = channels = 0;
  }
}

Texture::~Texture() { glDeleteTextures(1, &id); }

void Texture::Render(const Shader *shader, const glm::mat4 &model) {
  shader->Use();
  const auto &mvp = GetGameWindow().GetProjection() *
                    GetCamera().GetView() * model;
  shader->SetUniform("mvp", mvp);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, id);
  core::quad::Render(core::quad::QuadType::WITH_TEXCOORDS);
}
