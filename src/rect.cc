#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "rect.h"
#include <glm/gtc/matrix_transform.hpp>

void Rect::Render(unsigned int vertex_attrib, const Shader& shader, const glm::mat4& projection, const glm::mat4& view) {
  shader.Use();
  shader.SetUniform("projection", projection);
  shader.SetUniform("view", view);
  glm::mat4 model = glm::mat4(1.0f);
  model = glm::translate(model, glm::vec3(position, 0.0f));
  model = glm::scale(model, glm::vec3(scale, 1.0f));
  shader.SetUniform("model", model);
  shader.SetUniform("rect_color", color);
  glBindVertexArray(vertex_attrib);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}