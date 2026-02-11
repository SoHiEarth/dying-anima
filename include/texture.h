#pragma once
#include <string>
#include <glm/glm.hpp>
struct Shader;

struct Texture {
  unsigned int id;
  std::string path;
  int width;
  int height;
  int channels;
  Texture(std::string_view path);
  ~Texture();
  void Render(
    const Shader& shader,
    const glm::mat4& projection,
    const glm::mat4& view,
    const glm::mat4& model
  );
};