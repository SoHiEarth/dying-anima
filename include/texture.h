#pragma once
#include <glm/glm.hpp>
#include <string>
struct Shader;

struct Texture {
  unsigned int id;
  std::string path;
  int width;
  int height;
  int channels;
  Texture(std::string_view path);
  ~Texture();
  void Render(const Shader *shader, const glm::mat4 &model);
};
