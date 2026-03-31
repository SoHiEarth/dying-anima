#ifndef CORE_TEXTURE_H
#define CORE_TEXTURE_H

#include <glm/glm.hpp>
#include <string>
struct Shader;

struct Texture {
  unsigned int id;
  std::string path;
  int width;
  int height;
  int channels;
  explicit Texture(std::string_view path);
  ~Texture();
  void Render(const std::shared_ptr<Shader>& shader,
              const glm::mat4& model) const;
};

#endif  // CORE_TEXTURE_H
