#pragma once
#include <string>

struct Shader {
  unsigned int id;
  Shader(std::string_view vertex_path, std::string_view fragment_path);
  ~Shader();
  void Use() const;
  template <typename T>
  void SetUniform(std::string_view name, const T &value) const;
};