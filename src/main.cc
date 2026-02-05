#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <fstream>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <print>

void FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
}

int main() {
  if (!glfwInit()) {
    std::print("Failed to initialize GLFW\n");
    return -1;
  }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  GLFWwindow* window = glfwCreateWindow(800, 600, "Dying Anima", nullptr, nullptr);
  if (!window) {
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
  gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
  glViewport(0, 0, 800, 600);
  glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);

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
    return -1;
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
    return -1;
  }
  unsigned int shader_program = glCreateProgram();
  glAttachShader(shader_program, vertex_shader);
  glAttachShader(shader_program, fragment_shader);
  glLinkProgram(shader_program);
  glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(shader_program, 512, nullptr, infoLog);
    std::print("Shader Program Linking Error: {}", infoLog);
    return -1;
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
  glGenTextures(1, &banner_texture);
  glBindTexture(GL_TEXTURE_2D, banner_texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  int width, height, nrChannels;
  stbi_set_flip_vertically_on_load(true);
  unsigned char* data = stbi_load("assets/banner.png", &width, &height, &nrChannels, 0);
  if (data) {
    if (nrChannels == 1)
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, data);
    else if (nrChannels == 3)
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    else if (nrChannels == 4)
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    return -1;
  }
  stbi_image_free(data);
  glBindVertexArray(vertex_attrib);
  glBindTexture(GL_TEXTURE_2D, banner_texture);

  while (!glfwWindowShouldClose(window)) {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(shader_program);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, banner_texture);
    glBindVertexArray(vertex_attrib);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteTextures(1, &banner_texture);
  glDeleteProgram(shader_program);
  glDeleteBuffers(1, &vertex_buffer);
  glDeleteBuffers(1, &index_buffer);
  glDeleteVertexArrays(1, &vertex_attrib);
  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}