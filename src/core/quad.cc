#include "core/quad.h"

#include <glad/glad.h>

namespace core::quad::internal {
unsigned int full_quad_vertex_attrib, full_quad_vertex_buffer,
    full_quad_index_buffer;
unsigned int quad_vertex_attrib, quad_vertex_buffer, quad_index_buffer;
unsigned int tex_vertex_attrib, tex_vertex_buffer, tex_index_buffer;
} // namespace core::quad::internal

const float tex_vertices[] = {0.5f, 0.5f,  0.0f, 1.0f,  1.0f,  0.5f, -0.5f,
                              0.0f, 1.0f,  0.0f, -0.5f, -0.5f, 0.0f, 0.0f,
                              0.0f, -0.5f, 0.5f, 0.0f,  0.0f,  1.0f};

const float quad_vertices[] = {0.5f,  0.5f,  0.0f, 0.5f,  -0.5f, 0.0f,
                               -0.5f, -0.5f, 0.0f, -0.5f, 0.5f,  0.0f};

const float full_vertices[] = {1.0f,  1.0f,  0.0f,  1.0f,  1.0f,
                               1.0f, -1.0f,  0.0f,  1.0f,  0.0f,
                              -1.0f, -1.0f,  0.0f,  0.0f,  0.0f,
                              -1.0f,  1.0f,  0.0f,  0.0f,  1.0f};

const unsigned int indices[] = {0, 1, 3, 1, 2, 3};

void core::quad::Init() {
  glGenVertexArrays(1, &internal::tex_vertex_attrib);
  glGenBuffers(1, &internal::tex_vertex_buffer);
  glGenBuffers(1, &internal::tex_index_buffer);
  glBindVertexArray(internal::tex_vertex_attrib);
  glBindBuffer(GL_ARRAY_BUFFER, internal::tex_vertex_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(tex_vertices), tex_vertices,
               GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, internal::tex_index_buffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  glGenVertexArrays(1, &internal::quad_vertex_attrib);
  glGenBuffers(1, &internal::quad_vertex_buffer);
  glGenBuffers(1, &internal::quad_index_buffer);
  glBindVertexArray(internal::quad_vertex_attrib);
  glBindBuffer(GL_ARRAY_BUFFER, internal::quad_vertex_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), quad_vertices,
               GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, internal::quad_index_buffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  glGenVertexArrays(1, &internal::full_quad_vertex_attrib);
  glGenBuffers(1, &internal::full_quad_vertex_buffer);
  glGenBuffers(1, &internal::full_quad_index_buffer);
  glBindVertexArray(internal::full_quad_vertex_attrib);
  glBindBuffer(GL_ARRAY_BUFFER, internal::full_quad_vertex_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(full_vertices), full_vertices,
               GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, internal::full_quad_index_buffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

void core::quad::Render(QuadType type) {
  switch (type) {
  case QuadType::QUAD_ONLY:
    glBindVertexArray(internal::quad_vertex_attrib);
    break;
  case QuadType::WITH_TEXCOORDS:
    glBindVertexArray(internal::tex_vertex_attrib);
    break;
  case QuadType::FULL_QUAD:
    glBindVertexArray(internal::full_quad_vertex_attrib);
    break;
  }
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void core::quad::Quit() {
  glDeleteBuffers(1, &internal::full_quad_vertex_buffer);
  glDeleteBuffers(1, &internal::full_quad_index_buffer);
  glDeleteBuffers(1, &internal::quad_vertex_buffer);
  glDeleteBuffers(1, &internal::quad_index_buffer);
  glDeleteBuffers(1, &internal::tex_vertex_buffer);
  glDeleteBuffers(1, &internal::tex_index_buffer);
  glDeleteVertexArrays(1, &internal::full_quad_vertex_attrib);
  glDeleteVertexArrays(1, &internal::quad_vertex_attrib);
  glDeleteVertexArrays(1, &internal::tex_vertex_attrib);
}