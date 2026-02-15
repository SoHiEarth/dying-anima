#include "shader.h"

#include <glad/glad.h>

#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <iterator>
#include <print>
#include <string>

Shader::Shader(std::string_view vertex_path, std::string_view fragment_path) {
  unsigned int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  std::ifstream file(vertex_path.data());
  std::string vertex_shader_source((std::istreambuf_iterator<char>(file)),
                                   std::istreambuf_iterator<char>());
  const char* vertex_shader_source_cstr = vertex_shader_source.c_str();
  glShaderSource(vertex_shader, 1, &vertex_shader_source_cstr, nullptr);
  glCompileShader(vertex_shader);
  int success;
  char infoLog[512];
  glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertex_shader, 512, nullptr, infoLog);
    std::print("Vertex Shader Compilation Error: {}", infoLog);
  }
  unsigned int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  file = std::ifstream(fragment_path.data());
  std::string fragment_shader_source((std::istreambuf_iterator<char>(file)),
                                     std::istreambuf_iterator<char>());
  const char* fragment_shader_source_cstr = fragment_shader_source.c_str();
  glShaderSource(fragment_shader, 1, &fragment_shader_source_cstr, nullptr);
  glCompileShader(fragment_shader);
  glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragment_shader, 512, nullptr, infoLog);
    std::print("Fragment Shader Compilation Error: {}", infoLog);
  }
  id = glCreateProgram();
  glAttachShader(id, vertex_shader);
  glAttachShader(id, fragment_shader);
  glLinkProgram(id);
  glGetProgramiv(id, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(id, 512, nullptr, infoLog);
    std::print("Shader Program Linking Error: {}", infoLog);
  }
  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);
}

void Shader::Use() const { glUseProgram(id); }

template <>
void Shader::SetUniform(std::string_view name, const int& value) const {
  glUniform1i(glGetUniformLocation(id, name.data()), value);
}

template <>
void Shader::SetUniform(std::string_view name, const float& value) const {
  glUniform1f(glGetUniformLocation(id, name.data()), value);
}

template <>
void Shader::SetUniform(std::string_view name, const glm::vec2& value) const {
  glUniform2fv(glGetUniformLocation(id, name.data()), 1, glm::value_ptr(value));
}

template <>
void Shader::SetUniform(std::string_view name, const glm::vec3& value) const {
  glUniform3fv(glGetUniformLocation(id, name.data()), 1, glm::value_ptr(value));
}

template <>
void Shader::SetUniform(std::string_view name, const glm::vec4& value) const {
  glUniform4fv(glGetUniformLocation(id, name.data()), 1, glm::value_ptr(value));
}

template <>
void Shader::SetUniform(std::string_view name, const glm::mat4& value) const {
  glUniformMatrix4fv(glGetUniformLocation(id, name.data()), 1, GL_FALSE,
                     glm::value_ptr(value));
}