#include <glad/glad.h>
#include "menu.h"
#include <GLFW/glfw3.h>
#include <print>
#include <fstream>
#include <stb_image.h>
#include <thread>
#include <chrono>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

GameState Menu(GameState& state, GLFWwindow* window) {
  unsigned int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  std::ifstream file("assets/shaders/simple.vert.glsl");
  std::string vertex_shader_source((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
  const char* vertex_shader_source_cstr = vertex_shader_source.c_str();
  glShaderSource(vertex_shader, 1, &vertex_shader_source_cstr, nullptr);
  glCompileShader(vertex_shader);
  int success;
  char infoLog[512];
  glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertex_shader, 512, nullptr, infoLog);
    std::print("Vertex Shader Compilation Error: {}", infoLog);
    return GameState::MENU;
  }
  unsigned int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  file = std::ifstream("assets/shaders/simple.frag.glsl");
  std::string fragment_shader_source((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
  const char* fragment_shader_source_cstr = fragment_shader_source.c_str();
  glShaderSource(fragment_shader, 1, &fragment_shader_source_cstr, nullptr);
  glCompileShader(fragment_shader);
  glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragment_shader, 512, nullptr, infoLog);
    std::print("Fragment Shader Compilation Error: {}", infoLog);
    return GameState::MENU;
  }
  unsigned int shader_program = glCreateProgram();
  glAttachShader(shader_program, vertex_shader);
  glAttachShader(shader_program, fragment_shader);
  glLinkProgram(shader_program);
  glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(shader_program, 512, nullptr, infoLog);
    std::print("Shader Program Linking Error: {}", infoLog);
    return GameState::MENU;
  }
  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);
  glUseProgram(shader_program);
  glUniform1i(glGetUniformLocation(shader_program, "texture1"), 0);

  unsigned int vertex_attrib, vertex_buffer, index_buffer;
  float vertices[] = {
     1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
     1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
    -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
    -1.0f,  1.0f, 0.0f, 0.0f, 1.0f
  };
  unsigned int indices[] = {
    0, 1, 3,
    1, 2, 3
  };
  glGenVertexArrays(1, &vertex_attrib);
  glGenBuffers(1, &vertex_buffer);
  glGenBuffers(1, &index_buffer);
  glBindVertexArray(vertex_attrib);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  unsigned int banner_texture;
  {
    glGenTextures(1, &banner_texture);
    glBindTexture(GL_TEXTURE_2D, banner_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    int image_width, image_height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load("assets/banner.png", &image_width, &image_height, &nrChannels, 0);
    if (data) {
      if (nrChannels == 1)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, image_width, image_height, 0, GL_RED, GL_UNSIGNED_BYTE, data);
      else if (nrChannels == 3)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_width, image_height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
      else if (nrChannels == 4)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
      glGenerateMipmap(GL_TEXTURE_2D);
    } else {
      return GameState::MENU;
    }
    stbi_image_free(data);
    glBindVertexArray(vertex_attrib);
    glBindTexture(GL_TEXTURE_2D, banner_texture);
  }

  while (!glfwWindowShouldClose(window) && state == GameState::MENU) {
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    // Window is minimized, wait
    while (width == 0 || height == 0) {
      glfwGetFramebufferSize(window, &width, &height);
      glfwPollEvents();
    }
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -1.0f));
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 mvp = projection * view * model;
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(shader_program);
    glGetUniformLocation(shader_program, "mvp");
    glUniformMatrix4fv(glGetUniformLocation(shader_program, "mvp"), 1, GL_FALSE, glm::value_ptr(mvp));
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, banner_texture);
    glBindVertexArray(vertex_attrib);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  glDeleteTextures(1, &banner_texture);
  glDeleteBuffers(1, &vertex_buffer);
  glDeleteBuffers(1, &index_buffer);
  glDeleteVertexArrays(1, &vertex_attrib);
  glDeleteProgram(shader_program);
  state = GameState::EXIT;
  return state;
}